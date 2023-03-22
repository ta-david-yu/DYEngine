#include "Serialization/SerializedObjectFactory.h"

#include "Entity.h"

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

	SerializedEntity SerializedObjectFactory::CreateSerializedEntity(const DYEntity::Entity &entity)
	{
		// TODO:
		return SerializedEntity();
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