#include "Serialization/SerializedScene.h"

namespace DYE::DYEditor
{
	constexpr const char* ArrayOfEntityTablesKey = "Entities";
	constexpr const char* ArrayOfSystemTablesKey = "Systems";

	std::vector<SerializedSystemHandle> SerializedScene::GetSerializedSystemHandles()
	{
		toml::node* pArrayOfSystemTables = m_SceneTable.get(ArrayOfSystemTablesKey);

		if (pArrayOfSystemTables == nullptr || !pArrayOfSystemTables->is_array_of_tables())
		{
			// There is no array of tables with the given key name, we assume it is empty.
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
		toml::node* pArrayOfEntityTables = m_SceneTable.get(ArrayOfEntityTablesKey);

		if (pArrayOfEntityTables == nullptr || !pArrayOfEntityTables->is_array_of_tables())
		{
			// There is no array of tables with the given key name, we assume it is empty.
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

}