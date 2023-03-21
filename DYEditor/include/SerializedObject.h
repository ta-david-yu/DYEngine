#pragma once

#include <string>
#include <vector>
#include <memory>
#include <filesystem>
#include <optional>

#include <toml++/toml.h>

namespace DYE::DYEditor
{
	struct SerializedComponentHandle
	{
		friend class SerializedEntity;
		SerializedComponentHandle() = delete;

		std::optional<std::string> GetTypeName() const;
		void SetTypeName(std::string const& typeName);

	private:
		explicit SerializedComponentHandle(toml::table* pComponentTable);
		toml::table* m_pComponentTable = nullptr;
	};

	struct SerializedSystemHandle
	{
		friend class SerializedScene;
		SerializedSystemHandle() = delete;

	private:
		explicit SerializedSystemHandle(toml::table* pSystemTable);
		toml::table* m_pSystemTable = nullptr;
	};

	struct SerializedEntity
	{
		friend class SerializedObjectFactory;
		friend class SerializedScene;

		inline bool IsHandle() const { return m_IsHandle; }
		std::vector<SerializedComponentHandle> GetSerializedComponentHandles();

	private:
		SerializedEntity() = default;
		explicit SerializedEntity(toml::table* pEntityTableHandle);
		explicit SerializedEntity(toml::table&& entityTable);

		// This will only be valid when IsHandle is false
		toml::table m_EntityTable;

		// This will only be valid when IsHandle is true. In that case it points to the entity table owned by the belonging SerializedScene.
		toml::table* m_pEntityTableHandle = nullptr;

		bool m_IsHandle = false;
	};

	struct SerializedScene
	{
		friend class SerializedObjectFactory;

		std::optional<std::string> GetName() const;
		void SetName(std::string const& name);

		std::vector<SerializedSystemHandle> GetSerializedSystemHandles();
		std::vector<SerializedEntity> GetSerializedEntityHandles();

	private:
		SerializedScene() = default;
		explicit SerializedScene(toml::table&& sceneTable);
		toml::table m_SceneTable;
	};

	class SerializedObjectFactory
	{
	public:
		static std::optional<SerializedScene> GetSerializedSceneFromFile(std::filesystem::path const& path);
		static std::optional<SerializedEntity> GetSerializedEntityFromFile(std::filesystem::path const& path);
		static SerializedEntity CreateEmptySerializedEntity();
		static SerializedScene CreateEmptySerializedScene();
	};
}