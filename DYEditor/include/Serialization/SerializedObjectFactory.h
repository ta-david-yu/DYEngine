#pragma once

#include "SerializedEntity.h"
#include "SerializedScene.h"

#include <vector>
#include <string>
#include <filesystem>
#include <optional>

namespace DYE::DYEntity
{
	class Entity;
}

namespace DYE::DYEditor
{
	class Scene;

	class SerializedObjectFactory
	{
	public:
		/// SceneFile -> SerializedScene
		static std::optional<SerializedScene> LoadSerializedSceneFromFile(std::filesystem::path const& path);

		/// EntityFile -> SerializedEntity
		static std::optional<SerializedEntity> LoadSerializedEntityFromFile(std::filesystem::path const& path);

		/// SerializedScene -> Scene. \n
		/// This function assumes the given Scene is empty and doesn't do any clean up on the Scene.
		static void ApplySerializedSceneToEmptyScene(SerializedScene& serializedScene, DYE::DYEditor::Scene& scene);

		/// SerializedEntity -> Entity. \n
		/// This function assumes the given Entity is empty and doesn't do any clean up on the Entity.
		static void ApplySerializedEntityToEmptyEntity(SerializedEntity& serializedEntity, DYE::DYEntity::Entity& entity);

		/// Scene -> SerializedScene
		static SerializedScene CreateSerializedScene(Scene& scene);

		/// Entity -> SerializedEntity
		static SerializedEntity CreateSerializedEntity(DYE::DYEntity::Entity& entity);

		/// SerializedScene -> SceneFile
		static void SaveSerializedSceneToFile(SerializedScene& serializedScene, std::filesystem::path const& path);

		/// SerializedEntity -> EntityFile
		static void SaveSerializedEntityToFile(SerializedEntity& serializedEntity, std::filesystem::path const& path);

		static SerializedScene CreateEmptySerializedScene();
		static SerializedEntity CreateEmptySerializedEntity();
	};
}