#include "Serialization/SerializedObjectFactory.h"

#include "Type/TypeRegistry.h"
#include "Core/Entity.h"
#include "Core/Scene.h"

#include <fstream>
#include <toml++/toml.h>

namespace DYE::DYEditor
{
	std::optional<SerializedScene> SerializedObjectFactory::TryLoadSerializedSceneFromFile(const std::filesystem::path &path)
	{
		auto result = toml::parse_file(path.string());
		if (!result)
		{
			return {};
		}

		return SerializedScene(std::move(result.table()));
	}

	std::optional<SerializedEntity> SerializedObjectFactory::TryLoadSerializedEntityFromFile(const std::filesystem::path &path)
	{
		auto result = toml::parse_file(path.string());
		if (!result)
		{
			return {};
		}

		return SerializedEntity(std::move(result.table()));
	}

	void SerializedObjectFactory::ApplySerializedSceneToEmptyScene(SerializedScene &serializedScene, Scene &scene)
	{
#if DYE_DEBUG
		bool const isEmptyScene = scene.World.IsEmpty() && scene.InitializeSystemDescriptors.empty();
		DYE_ASSERT(isEmptyScene && "The given scene is not empty!");
#endif

		auto serializedSystemHandles = serializedScene.GetSerializedSystemHandles();
		auto serializedEntityHandles = serializedScene.GetSerializedEntityHandles();

		auto tryGetSceneNameResult = serializedScene.TryGetName();
		if (tryGetSceneNameResult.has_value())
		{
			scene.Name = tryGetSceneNameResult.value();
		}

		// Populate systems.
		for (auto& serializedSystemHandle : serializedSystemHandles)
		{
			auto getTypeNameResult = serializedSystemHandle.TryGetTypeName();
			if (!getTypeNameResult.has_value())
			{
				continue;
			}

			auto getGroupResult = serializedSystemHandle.TryGetGroupName();
			bool const hasGroup = getGroupResult.has_value();
			SystemDescriptor systemDescriptor =
				{
					.Name = getTypeNameResult.value(),
					.Group = hasGroup? scene.AddOrGetGroupID(getGroupResult.value()) : NoSystemGroupID,
					.IsEnabled = serializedSystemHandle.GetIsEnabledOr(true)
				};

			SystemBase* pSystemInstance = TypeRegistry::TryGetSystemInstance(getTypeNameResult.value());
			if (pSystemInstance != nullptr)
			{
				systemDescriptor.Instance = pSystemInstance;

				auto const phase = pSystemInstance->GetPhase();
				scene.GetSystemDescriptorsOfPhase(phase).emplace_back(systemDescriptor);
			}
			else
			{
				// Unrecognized system in TypeRegistry.
				scene.UnrecognizedSystems.emplace_back(systemDescriptor);
			}
		}

		// Populate entities.
		scene.World.Reserve(serializedEntityHandles.size());
		for (int i = serializedEntityHandles.size() - 1; i >= 0; i--)
		{
			auto& serializedEntityHandle = serializedEntityHandles[i];
			DYEditor::Entity entity = scene.World.CreateEntity();
			ApplySerializedEntityToEmptyEntity(serializedEntityHandle, entity);
		}
	}

	void SerializedObjectFactory::ApplySerializedEntityToEmptyEntity(SerializedEntity &serializedEntity,
																	 DYEditor::Entity &entity)
	{
		std::vector<SerializedComponentHandle> serializedComponentHandles = serializedEntity.GetSerializedComponentHandles();
		for (auto& serializedComponentHandle : serializedComponentHandles)
		{
			auto getTypeNameResult = serializedComponentHandle.TryGetTypeName();
			if (!getTypeNameResult.has_value())
			{
				// Garbage component element without a type name, skip it.
				continue;
			}

			auto& typeName = getTypeNameResult.value();
			auto getComponentTypeFunctionsResult = TypeRegistry::TryGetComponentTypeDescriptor(typeName);
			if (!getComponentTypeFunctionsResult.has_value())
			{
				// Cannot find the given component type and its related functions, add the component name
				// to the unrecognized component list OR component.
				// TODO: Keep track of unrecognized component so we could show it in the entity inspector.
				DYE_LOG("Entity has an unrecognized component of type '%s'.", typeName.c_str());
				continue;
			}

			auto& componentTypeFunctions = getComponentTypeFunctionsResult.value();
			if (componentTypeFunctions.Deserialize == nullptr)
			{
				// The component type doesn't have a corresponding Deserialize function.
				// // TODO: Ignore and add the component name to the unrecognized component list.
				DYE_LOG("Component of type '%s' will not be deserialized because its Deserialize function is not provided.", typeName.c_str());
				continue;
			}

			try
			{
				DeserializationResult const result = componentTypeFunctions.Deserialize(serializedComponentHandle, entity);
			}
			catch (std::exception& exception)
			{
				DYE_LOG_ERROR(exception.what());
				DYE_ASSERT(false);
			}
		}
	}

	SerializedScene SerializedObjectFactory::CreateSerializedScene(Scene &scene)
	{
		SerializedScene serializedScene;
		serializedScene.SetName(scene.Name);

		// Populate systems.
		scene.ForEachSystemDescriptor
			(
				[&serializedScene, &scene](SystemDescriptor const &systemDescriptor, ExecutionPhase phase)
				{
					serializedScene.TryAddSystem
						(
							SerializedScene::AddSystemParameters
								{
									.SystemTypeName = systemDescriptor.Name,
									.HasGroup = systemDescriptor.Group != NoSystemGroupID,
									.SystemGroupName = (systemDescriptor.Group != NoSystemGroupID)
													   ? scene.SystemGroupNames[systemDescriptor.Group] : "",
									.IsEnabled = systemDescriptor.IsEnabled
								}
						);
				}
			);

		// Populate unrecognized/unknown systems.
		for (auto unrecognizedSystemDescriptor : scene.UnrecognizedSystems)
		{
			serializedScene.TryAddSystem
				(
					SerializedScene::AddSystemParameters
						{
							.SystemTypeName = unrecognizedSystemDescriptor.Name,
							.HasGroup = unrecognizedSystemDescriptor.Group != NoSystemGroupID,
							.SystemGroupName = (unrecognizedSystemDescriptor.Group != NoSystemGroupID) ? scene.SystemGroupNames[unrecognizedSystemDescriptor.Group] : "",
							.IsEnabled = unrecognizedSystemDescriptor.IsEnabled
						}
				);
		}

		// Populate entities and their components.
		scene.World.ForEachEntity
		(
			[&serializedScene](auto& entity)
			{
				SerializedEntity serializedEntity = serializedScene.CreateAndAddSerializedEntity();

				auto componentNamesAndFunctions = TypeRegistry::GetComponentTypesNamesAndDescriptors();
				for (auto& [name, functions] : componentNamesAndFunctions)
				{
					if (!functions.Has(entity))
					{
						continue;
					}

					SerializedComponentHandle serializedComponent = serializedEntity.TryAddComponentOfType(name);
					if (functions.Serialize == nullptr)
					{
						// A 'Serialize' function is not provided for the given component type. Skip the process.
						continue;
					}

					SerializationResult const result = functions.Serialize(entity, serializedComponent);
				}
			}
		);

		return serializedScene;
	}

	SerializedEntity SerializedObjectFactory::CreateSerializedEntity(DYE::DYEditor::Entity& entity)
	{
		SerializedEntity serializedEntity;

		auto componentNamesAndFunctions = TypeRegistry::GetComponentTypesNamesAndDescriptors();
		for (auto& [name, functions] : componentNamesAndFunctions)
		{
			if (!functions.Has(entity))
			{
				continue;
			}

			SerializedComponentHandle serializedComponent = serializedEntity.TryAddComponentOfType(name);
			if (functions.Serialize == nullptr)
			{
				// A 'Serialize' function is not provided for the given component type. Skip the process.
				continue;
			}

			SerializationResult const result = functions.Serialize(entity, serializedComponent);
		}

		return serializedEntity;
	}

	void SerializedObjectFactory::SaveSerializedEntityToFile(SerializedEntity &serializedEntity,
															 const std::filesystem::path &path)
	{
		std::ofstream fileStream(path, std::ios::trunc);
		if (serializedEntity.IsHandle())
		{
			// TODO: do some extra work to include children entities (i.e. nested prefab)
			fileStream << *serializedEntity.m_pEntityTableHandle;
		}
		else
		{
			fileStream << serializedEntity.m_EntityTable;
		}
	}

	void SerializedObjectFactory::SaveSerializedSceneToFile(SerializedScene &serializedScene,
															const std::filesystem::path &path)
	{
		std::ofstream fileStream(path, std::ios::trunc);
		fileStream << serializedScene.m_SceneTable;
	}

	SerializedEntity SerializedObjectFactory::CreateEmptySerializedEntity()
	{
		return {};
	}

	SerializedScene SerializedObjectFactory::CreateEmptySerializedScene()
	{
		return {};
	}
}