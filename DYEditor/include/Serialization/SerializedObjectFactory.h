#pragma once

#include "SerializedEntity.h"
#include "SerializedScene.h"

#include <vector>
#include <string>
#include <filesystem>
#include <optional>


namespace DYE::DYEditor
{
	class Entity;
	class Scene;
	struct ComponentTypeDescriptor;

	struct EntityDeserializationResult
	{
		std::vector<std::string> UnrecognizedComponentTypeNames {};
		std::vector<SerializedComponent> UnrecognizedSerializedComponentHandles {};
		bool Success = true;
	};

	class SerializedObjectFactory
	{
	public:
		/// SceneFile -> SerializedScene
		static std::optional<SerializedScene> TryLoadSerializedSceneFromFile(std::filesystem::path const& path);

		/// EntityFile -> SerializedEntity
		static std::optional<SerializedEntity> TryLoadSerializedEntityFromFile(std::filesystem::path const& path);

		/// SerializedScene -> Scene. \n
		/// This function assumes the given Scene is empty and doesn't do any clean-up on the Scene.
		static void ApplySerializedSceneToEmptyScene(SerializedScene& serializedScene, DYE::DYEditor::Scene& scene);

		/// SerializedEntity -> Entity. \n
		/// This function assumes the given Entity is empty and doesn't do any clean-up on the Entity.
		static EntityDeserializationResult ApplySerializedEntityToEmptyEntity(SerializedEntity& serializedEntity, DYE::DYEditor::Entity& entity);

		/// Scene -> SerializedScene
		static SerializedScene CreateSerializedScene(Scene& scene);

		/// Entity -> SerializedEntity
		static SerializedEntity CreateSerializedEntity(DYE::DYEditor::Entity& entity);

		/// SerializedScene -> SceneFile
		static void SaveSerializedSceneToFile(SerializedScene& serializedScene, std::filesystem::path const& path);

		/// SerializedEntity -> EntityFile
		static void SaveSerializedEntityToFile(SerializedEntity& serializedEntity, std::filesystem::path const& path);

		/// Entity.Component -> SerializedComponent
		static SerializedComponent CreateSerializedComponentOfType(DYE::DYEditor::Entity &entity,
																   std::string const &componentTypeName,
																   ComponentTypeDescriptor componentTypeDescriptor);

		static SerializedScene CreateEmptySerializedScene();
		static SerializedEntity CreateEmptySerializedEntity();
		static SerializedComponent CreateEmptySerializedComponent();
	};
}