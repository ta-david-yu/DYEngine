#include "Serialization/SerializedObjectFactory.h"

#include "TypeRegistry.h"
#include "Entity.h"

#include <fstream>
#include <toml++/toml.h>

namespace DYE::DYEditor
{
	std::optional<SerializedScene> SerializedObjectFactory::GetSerializedSceneFromFile(const std::filesystem::path &path)
	{
		auto result = toml::parse_file(path.string());
		if (!result)
		{
			// TODO: log error message here
			return {};
		}

		return SerializedScene(std::move(result.table()));
	}

	std::optional<SerializedEntity> SerializedObjectFactory::GetSerializedEntityFromFile(const std::filesystem::path &path)
	{
		auto result = toml::parse_file(path.string());
		if (!result)
		{
			// TODO: log error message here
			return {};
		}

		return SerializedEntity(std::move(result.table()));
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

			if (functions.Serialize == nullptr)
			{
				// A 'Serialize' function is not provided for the given component type. Skip the process.
				serializedEntity.TryAddComponentOfType(name);
				continue;
			}

			SerializationResult const result = functions.Serialize(entity, serializedEntity);
		}

		return serializedEntity;
	}

	SerializedEntity SerializedObjectFactory::CreateEmptySerializedEntity()
	{
		return {};
	}

	SerializedScene SerializedObjectFactory::CreateEmptySerializedScene()
	{
		return {};
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
}