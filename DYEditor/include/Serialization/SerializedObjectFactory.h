#pragma once

#include "SerializedEntity.h"
#include "SerializedScene.h"

#include <filesystem>
#include <optional>

namespace DYE::DYEntity
{
	class Entity;
}

namespace DYE::DYEditor
{
	class SerializedObjectFactory
	{
	public:
		static std::optional<SerializedScene> GetSerializedSceneFromFile(std::filesystem::path const& path);
		static std::optional<SerializedEntity> GetSerializedEntityFromFile(std::filesystem::path const& path);

		static SerializedEntity CreateSerializedEntity(DYE::DYEntity::Entity& entity);
		// TODO:
		// 		static SerializedScene CreateSerializedScene(layer? world?)

		static SerializedEntity CreateEmptySerializedEntity();
		static SerializedScene CreateEmptySerializedScene();

		static void SaveSerializedEntityToFile(SerializedEntity& serializedEntity, std::filesystem::path const& path);
	};
}