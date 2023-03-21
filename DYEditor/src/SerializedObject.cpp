#include "SerializedObject.h"

#include <toml++/toml.h>

namespace DYE::DYEditor
{
	constexpr const char* k_ArrayOfEntityTablesKey = "Entities";
	constexpr const char* k_ArrayOfComponentTablesKey = "Components";
	constexpr const char* k_ArrayOfSystemTablesKey = "Systems";

	std::optional<std::string> SerializedComponentHandle::GetTypeName() const
	{
		auto pTypeNode = m_pComponentTable->get("Type");
		if (pTypeNode == nullptr)
		{
			return {};
		}

		return pTypeNode->value<std::string>();
	}

	void SerializedComponentHandle::SetTypeName(std::string const& typeName)
	{
		m_pComponentTable->insert_or_assign("Type", typeName);
	}

	SerializedComponentHandle::SerializedComponentHandle(toml::table *pComponentTable) : m_pComponentTable(pComponentTable)
	{
	}

	SerializedSystemHandle::SerializedSystemHandle(toml::table *pSystemTable) : m_pSystemTable(pSystemTable)
	{
	}

	std::vector<SerializedComponentHandle> SerializedEntity::GetSerializedComponentHandles()
	{
		toml::node* pArrayOfComponentTables = nullptr;
		if (IsHandle())
		{
			pArrayOfComponentTables = m_pEntityTableHandle->get(k_ArrayOfComponentTablesKey);
		}
		else
		{
			pArrayOfComponentTables = m_EntityTable.get(k_ArrayOfComponentTablesKey);
		}

		if (pArrayOfComponentTables == nullptr)
		{
			// There is no node with the given key name, we assume it is empty.
			return {};
		}

		if (!pArrayOfComponentTables->is_array_of_tables())
		{
			// The target node is not an array of table, we assume it is empty.
			return {};
		}

		toml::array& arrayOfComponentTables = *pArrayOfComponentTables->as_array();

		std::vector<SerializedComponentHandle> handles;
		auto const numberOfComponents = arrayOfComponentTables.size();
		handles.reserve(numberOfComponents);

		for (int componentIndex = 0; componentIndex < numberOfComponents; componentIndex++)
		{
			toml::table* pComponentTable = arrayOfComponentTables[componentIndex].as_table();
			handles.push_back(SerializedComponentHandle(pComponentTable));
		}

		return handles;
	}

	SerializedEntity::SerializedEntity(toml::table *pEntityTableHandle) : m_pEntityTableHandle(pEntityTableHandle), m_IsHandle(true)
	{
	}

	SerializedEntity::SerializedEntity(toml::table &&entityTable) : m_EntityTable(entityTable), m_IsHandle(false)
	{
	}

	std::vector<SerializedSystemHandle> SerializedScene::GetSerializedSystemHandles()
	{
		toml::node* pArrayOfSystemTables = m_SceneTable.get(k_ArrayOfSystemTablesKey);

		if (pArrayOfSystemTables == nullptr)
		{
			// There is no node with the given key name, we assume it is empty.
			return {};
		}

		if (!pArrayOfSystemTables->is_array_of_tables())
		{
			// The target node is not an array of table, we assume it is empty.
			return {};
		}

		toml::array& arrayOfSystemTables = *pArrayOfSystemTables->as_array();

		std::vector<SerializedSystemHandle> handles;
		auto const numberOfSystems = arrayOfSystemTables.size();
		handles.reserve(numberOfSystems);

		for (int systemIndex = 0; systemIndex < numberOfSystems; systemIndex++)
		{
			toml::table* pSystemTable = arrayOfSystemTables[systemIndex].as_table();
			handles.push_back(SerializedSystemHandle(pSystemTable));
		}

		return handles;
	}

	std::vector<SerializedEntity> SerializedScene::GetSerializedEntityHandles()
	{
		toml::node* pArrayOfEntityTables = m_SceneTable.get(k_ArrayOfEntityTablesKey);

		if (pArrayOfEntityTables == nullptr)
		{
			// There is no node with the given key name, we assume it is empty.
			return {};
		}

		if (!pArrayOfEntityTables->is_array_of_tables())
		{
			// The target node is not an array of table, we assume it is empty.
			return {};
		}

		toml::array& arrayOfEntityTables = *pArrayOfEntityTables->as_array();

		std::vector<SerializedEntity> handles;
		auto const numberOfEntities = arrayOfEntityTables.size();
		handles.reserve(numberOfEntities);

		for (int entityIndex = 0; entityIndex < numberOfEntities; entityIndex++)
		{
			toml::table* pEntityTable = arrayOfEntityTables[entityIndex].as_table();
			handles.push_back(SerializedEntity(pEntityTable));
		}

		return handles;
	}

	SerializedScene::SerializedScene(toml::table &&sceneTable) : m_SceneTable(sceneTable)
	{
	}

	std::optional<std::string> SerializedScene::GetName() const
	{
		auto pNameNode = m_SceneTable.get("Name");
		if (pNameNode == nullptr)
		{
			return {};
		}

		return pNameNode->value<std::string>();
	}

	void SerializedScene::SetName(const std::string &name)
	{
		m_SceneTable.insert_or_assign("Name", name);
	}

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

	SerializedEntity SerializedObjectFactory::CreateEmptySerializedEntity()
	{
		return {};
	}

	SerializedScene SerializedObjectFactory::CreateEmptySerializedScene()
	{
		return {};
	}
}