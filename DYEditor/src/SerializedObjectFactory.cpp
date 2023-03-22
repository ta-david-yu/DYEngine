#include "Serialization/SerializedObjectFactory.h"

#include "TypeRegistry.h"
#include "Entity.h"
#include "Scene.h"

#include <fstream>
#include <toml++/toml.h>

namespace DYE::DYEditor
{
	std::optional<SerializedScene> SerializedObjectFactory::LoadSerializedSceneFromFile(const std::filesystem::path &path)
	{
		auto result = toml::parse_file(path.string());
		if (!result)
		{
			// TODO: log error message here
			return {};
		}

		return SerializedScene(std::move(result.table()));
	}

	std::optional<SerializedEntity> SerializedObjectFactory::LoadSerializedEntityFromFile(const std::filesystem::path &path)
	{
		auto result = toml::parse_file(path.string());
		if (!result)
		{
			// TODO: log error message here
			return {};
		}

		return SerializedEntity(std::move(result.table()));
	}

	SerializedScene SerializedObjectFactory::CreateSerializedScene(Scene &scene)
	{
		SerializedScene serializedScene;
		serializedScene.SetName(scene.Name);

		// Populate systems.
		for (auto systemName : scene.SystemTypeNames)
		{
			serializedScene.TryAddSystemOfType(systemName);
		}

		// Populate entities and their components.
		scene.World.ForEachEntity
		(
			[&serializedScene](auto& entity)
			{
				SerializedEntity serializedEntity = serializedScene.CreateAndAddSerializedEntity();

				auto componentNamesAndFunctions = TypeRegistry::GetComponentTypesNamesAndFunctionCollections();
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

	SerializedEntity SerializedObjectFactory::CreateSerializedEntity(DYE::DYEntity::Entity& entity)
	{
		SerializedEntity serializedEntity;

		auto componentNamesAndFunctions = TypeRegistry::GetComponentTypesNamesAndFunctionCollections();
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
			// TODO: do some extra work to include children entities
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