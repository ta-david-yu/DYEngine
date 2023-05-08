#include "Serialization/SerializedObjectFactory.h"

#include "Type/TypeRegistry.h"
#include "Core/Entity.h"
#include "Core/Scene.h"
#include "FileSystem/FileSystem.h"

#include <fstream>
#include <toml++/toml.h>

namespace DYE::DYEditor
{
	std::optional<SerializedScene> SerializedObjectFactory::TryLoadSerializedSceneFromFile(const std::filesystem::path &path)
	{
		if (!FileSystem::FileExists(path))
		{
			DYE_LOG("Cannot find the scene file: %s", path.string().c_str());
		}

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
		for (int i = 0; i < serializedEntityHandles.size(); i++)
		{
			auto& serializedEntityHandle = serializedEntityHandles[i];

			DYEditor::Entity entity = scene.World.createUntrackedEntity();
			auto result = ApplySerializedEntityToEmptyEntity(serializedEntityHandle, entity);

			if (!result.Success)
			{
				// If the deserialized entity has some issue during deserialization, add the result as component to the entity.
				entity.AddComponent<EntityDeserializationResult>(result);
			}

			auto tryGetGUID = entity.TryGetGUID();
			if (tryGetGUID.has_value())
			{
				scene.World.registerUntrackedEntityAtIndex(entity, i);
			}
			else
			{
				DYE_LOG("The entity at index %d doesn't have a GUID (IDComponent), it will not be tracked by the World.", i);
			}
		}
	}

	EntityDeserializationResult SerializedObjectFactory::ApplySerializedEntityToEmptyEntity(SerializedEntity &serializedEntity,
																	 DYEditor::Entity &entity)
	{
		EntityDeserializationResult result;

		std::vector<SerializedComponent> serializedComponentHandles = serializedEntity.GetSerializedComponentHandles();
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
				result.Success = false;
				result.UnrecognizedComponentTypeNames.push_back(typeName);
				result.UnrecognizedSerializedComponents.push_back(serializedComponentHandle.CloneAsNonHandle());
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

#if defined(__EXCEPTIONS)
			try
			{
				DeserializationResult const deserializeComponentResult = componentTypeFunctions.Deserialize(serializedComponentHandle, entity);
			}
			catch (std::exception& exception)
			{
				DYE_LOG_ERROR(exception.what());
				DYE_ASSERT(false);
			}
#else
			DeserializationResult const deserializeComponentResult = componentTypeFunctions.Deserialize(serializedComponentHandle, entity);
#endif
		}

		return std::move(result);
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
				serializedScene.AddOrReplaceSystemHandle
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
			serializedScene.AddOrReplaceSystemHandle
				(
					SerializedScene::AddSystemParameters
						{
							.SystemTypeName = unrecognizedSystemDescriptor.Name,
							.HasGroup = unrecognizedSystemDescriptor.Group != NoSystemGroupID,
							.SystemGroupName = (unrecognizedSystemDescriptor.Group != NoSystemGroupID)
											   ? scene.SystemGroupNames[unrecognizedSystemDescriptor.Group] : "",
							.IsEnabled = unrecognizedSystemDescriptor.IsEnabled
						}
				);
		}

		// Populate entities and their components.
		scene.World.ForEachEntity
		(
			[&serializedScene](auto& entity)
			{
				SerializedEntity serializedEntity = serializedScene.CreateAndAddEntityHandle();

				auto componentNamesAndFunctions = TypeRegistry::GetComponentTypesNamesAndDescriptors();
				for (auto& [name, functions] : componentNamesAndFunctions)
				{
					if (!functions.Has(entity))
					{
						continue;
					}

					SerializedComponent serializedComponent = serializedEntity.AddOrGetComponentHandleOfType(name);
					if (functions.Serialize == nullptr)
					{
						// A 'Serialize' function is not provided for the given component type. Skip the process.
						continue;
					}

					SerializationResult const result = functions.Serialize(entity, serializedComponent);
				}

				if (entity.template HasComponent<EntityDeserializationResult>())
				{
					// Emplace serialized unrecognized component data back.
					auto &deserializationResult = entity.template GetComponent<EntityDeserializationResult>();
					for (auto &serializedComponent : deserializationResult.UnrecognizedSerializedComponents)
					{
						serializedEntity.PushSerializedComponent(serializedComponent);
					}
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

			SerializedComponent serializedComponent = serializedEntity.AddOrGetComponentHandleOfType(name);
			if (functions.Serialize == nullptr)
			{
				// A 'Serialize' function is not provided for the given component type. Skip the process.
				continue;
			}

			SerializationResult const result = functions.Serialize(entity, serializedComponent);
		}

		if (entity.HasComponent<EntityDeserializationResult>())
		{
			// Emplace serialized unrecognized component data back.
			auto &deserializationResult = entity.GetComponent<EntityDeserializationResult>();
			for (auto &serializedComponent : deserializationResult.UnrecognizedSerializedComponents)
			{
				serializedEntity.PushSerializedComponent(serializedComponent);
			}
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

	SerializedComponent SerializedObjectFactory::CreateSerializedComponentOfType(Entity &entity,
																				 std::string const &componentTypeName,
																				 ComponentTypeDescriptor componentTypeDescriptor)
	{
		// Default serialized component ctor creates an empty non-handle serialized component.
		SerializedComponent serializedComponent;

		serializedComponent.SetTypeName(componentTypeName);
		componentTypeDescriptor.Serialize(entity, serializedComponent);
		return serializedComponent;
	}

	SerializedEntity SerializedObjectFactory::CreateEmptySerializedEntity()
	{
		return {};
	}

	SerializedScene SerializedObjectFactory::CreateEmptySerializedScene()
	{
		return {};
	}

	SerializedComponent SerializedObjectFactory::CreateEmptySerializedComponent()
	{
		return {};
	}
}