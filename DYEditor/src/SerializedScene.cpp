#include "Serialization/SerializedScene.h"

namespace DYE::DYEditor
{
	constexpr const char* ArrayOfEntityTablesKey = "Entities";
	constexpr const char* ArrayOfSystemTablesKey = "Systems";

	std::optional<std::string> SerializedScene::TryGetName() const
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

	SerializedEntity SerializedScene::CreateAndAddSerializedEntity()
	{
		toml::array* pArrayOfEntityTables = m_SceneTable.get_as<toml::array>(ArrayOfEntityTablesKey);

		if (pArrayOfEntityTables == nullptr)
		{
			// Emplace an entity array into the entity table if it doesn't exist yet.
			auto [iterator, insertionResult] = m_SceneTable.emplace(ArrayOfEntityTablesKey, toml::array());
			pArrayOfEntityTables = iterator->second.as_array();
		}

		toml::table* pNewEntityTableHandle = &pArrayOfEntityTables->emplace_back(toml::table());

		return SerializedEntity(pNewEntityTableHandle);
	}

	SerializedSystemHandle SerializedScene::TryAddSystem(AddSystemParameters addSystemParams)
	{
		toml::array* pArrayOfSystemTables = m_SceneTable.get_as<toml::array>(ArrayOfSystemTablesKey);

		bool needToCreateNewSystem = false;
		toml::table* pSystemTable = nullptr;
		if (pArrayOfSystemTables == nullptr)
		{
			// Emplace a system array into the entity table if it doesn't exist yet.
			auto [iterator, insertionResult] = m_SceneTable.emplace(ArrayOfSystemTablesKey, toml::array());
			pArrayOfSystemTables = iterator->second.as_array();
			needToCreateNewSystem = true;
		}
		else
		{
			// The array was created already, try to find the system of the given type in the array.
			auto systemNodeOfTypeItr =
				std::find_if(pArrayOfSystemTables->begin(), pArrayOfSystemTables->end(),
							 [&addSystemParams] (toml::node& systemNode)
							 {
								 toml::node* systemTypeNameNode = systemNode.as_table()->get(SystemTypeNameKey);
								 return systemTypeNameNode != nullptr && systemTypeNameNode->value<std::string>() == addSystemParams.SystemTypeName;
							 });

			bool const hasSystem = systemNodeOfTypeItr != pArrayOfSystemTables->end();
			if (hasSystem)
			{
				// Found the existing component, use it directly.
				pSystemTable = systemNodeOfTypeItr->as_table();
			}
			else
			{
				needToCreateNewSystem = true;
			}
		}

		if (needToCreateNewSystem)
		{
			// Create a new system (table) with the given type name.
			pSystemTable = &pArrayOfSystemTables->emplace_back(toml::table {{SystemTypeNameKey, addSystemParams.SystemTypeName}});
		}

		if (addSystemParams.HasGroup)
		{
			pSystemTable->insert_or_assign(SystemGroupNameKey, addSystemParams.SystemGroupName);
		}

		return SerializedSystemHandle(pSystemTable);
	}

	SerializedScene::SerializedScene(toml::table &&sceneTable) : m_SceneTable(sceneTable)
	{
	}
}