#include "Serialization/SerializedComponentHandle.h"

namespace DYE::DYEditor
{
	std::optional<std::string> SerializedComponentHandle::TryGetTypeName() const
	{
		auto pTypeNode = m_pComponentTable->get(ComponentTypeNameKey);
		if (pTypeNode == nullptr)
		{
			return {};
		}

		return pTypeNode->value<std::string>();
	}

	void SerializedComponentHandle::SetTypeName(std::string const& typeName)
	{
		m_pComponentTable->insert_or_assign(ComponentTypeNameKey, typeName);
	}

	SerializedComponentHandle::SerializedComponentHandle(toml::table *pComponentTable) : m_pComponentTable(pComponentTable)
	{
	}
}