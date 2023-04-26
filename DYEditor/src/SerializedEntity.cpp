#include "Serialization/SerializedEntity.h"

namespace DYE::DYEditor
{
	constexpr const char* ArrayOfComponentTablesKey = "Components";

	std::vector<SerializedComponent> SerializedEntity::GetSerializedComponentHandles()
	{
		toml::table* pEntityTable = IsHandle()? m_pEntityTableHandle : &m_EntityTable;
		toml::node* pArrayOfComponentTables = pEntityTable->get(ArrayOfComponentTablesKey);

		if (pArrayOfComponentTables == nullptr || !pArrayOfComponentTables->is_array_of_tables())
		{
			// There is no array of tables with the given key name, we assume it is empty.
			return {};
		}

		toml::array& arrayOfComponentTables = *pArrayOfComponentTables->as_array();

		std::vector<SerializedComponent> handles;
		auto const numberOfComponents = arrayOfComponentTables.size();
		handles.reserve(numberOfComponents);

		for (int componentIndex = 0; componentIndex < numberOfComponents; componentIndex++)
		{
			toml::table* pComponentTable = arrayOfComponentTables[componentIndex].as_table();
			handles.push_back(SerializedComponent(pComponentTable));
		}

		return handles;
	}

	bool SerializedEntity::TryRemoveComponentHandleOfType(const std::string &typeName)
	{
		toml::table* pEntityTable = IsHandle()? m_pEntityTableHandle : &m_EntityTable;
		toml::node* pArrayOfComponentTables = pEntityTable->get(ArrayOfComponentTablesKey);

		if (pArrayOfComponentTables == nullptr || !pArrayOfComponentTables->is_array_of_tables())
		{
			// There is no array of tables with the given key name, we assume it is empty.
			return {};
		}

		toml::array& arrayOfComponentTables = *pArrayOfComponentTables->as_array();
		auto componentNodeOfTypeItr =
			std::find_if(arrayOfComponentTables.begin(), arrayOfComponentTables.end(),
						 [typeName] (toml::node& componentNode)
						 {
							 toml::node* componentTypeNameNode = componentNode.as_table()->get(ComponentTypeNameKey);
							 return componentTypeNameNode != nullptr && componentTypeNameNode->value<std::string>() == typeName;
						 });

		if (componentNodeOfTypeItr == arrayOfComponentTables.end())
		{
			// The component node with the given typename is not found, skip it.
			return false;
		}

		// Erase the component with the given typename.
		arrayOfComponentTables.erase(componentNodeOfTypeItr);
		return true;
	}

	SerializedComponent SerializedEntity::TryAddOrGetComponentHandleOfType(const std::string &typeName)
	{
		toml::table* pEntityTable = IsHandle()? m_pEntityTableHandle : &m_EntityTable;
		toml::array* pArrayOfComponentTables = pEntityTable->get_as<toml::array>(ArrayOfComponentTablesKey);

		bool needToCreateNewComponent = false;
		toml::table* pComponentTable = nullptr;
		if (pArrayOfComponentTables == nullptr)
		{
			// Emplace a component array into the entity table if it doesn't exist yet.
			auto [iterator, insertionResult] = pEntityTable->emplace(ArrayOfComponentTablesKey, toml::array{});
			pArrayOfComponentTables = iterator->second.as_array();
			needToCreateNewComponent = true;
		}
		else
		{
			// The array was created already, try to find the component of the given type in the array.
			auto componentNodeOfTypeItr =
				std::find_if(pArrayOfComponentTables->begin(), pArrayOfComponentTables->end(),
							 [typeName] (toml::node& componentNode)
							 {
								 toml::node* componentTypeNameNode = componentNode.as_table()->get(ComponentTypeNameKey);
								 return componentTypeNameNode != nullptr && componentTypeNameNode->value<std::string>() == typeName;
							 });

			bool const hasComponent = componentNodeOfTypeItr != pArrayOfComponentTables->end();
			if (hasComponent)
			{
				// Found the existing component, use it directly.
				pComponentTable = componentNodeOfTypeItr->as_table();
			}
			else
			{
				needToCreateNewComponent = true;
			}
		}

		if (needToCreateNewComponent)
		{
			// Create a new component (table) with the given type name.
			pComponentTable = &pArrayOfComponentTables->emplace_back(toml::table {{ComponentTypeNameKey, typeName}});
		}

		return SerializedComponent(pComponentTable);
	}

	void SerializedEntity::PushSerializedComponent(const SerializedComponent &serializedComponent)
	{
		toml::table* pEntityTable = IsHandle()? m_pEntityTableHandle : &m_EntityTable;
		toml::array* pArrayOfComponentTables = pEntityTable->get_as<toml::array>(ArrayOfComponentTablesKey);

		toml::table const &componentTable = serializedComponent.IsHandle()? *serializedComponent.m_pComponentTableHandle : serializedComponent.m_ComponentTable;
		if (pArrayOfComponentTables == nullptr)
		{
			// Emplace a component array into the entity table if it doesn't exist yet.
			auto [iterator, insertionResult] = pEntityTable->emplace(ArrayOfComponentTablesKey, toml::array{});
			pArrayOfComponentTables = iterator->second.as_array();
		}

		pArrayOfComponentTables->push_back(componentTable);
	}

	SerializedEntity::SerializedEntity(toml::table *pEntityTableHandle) : m_pEntityTableHandle(pEntityTableHandle), m_IsHandle(true)
	{
	}

	SerializedEntity::SerializedEntity(toml::table &&entityTable) : m_EntityTable(entityTable), m_IsHandle(false)
	{
	}
}