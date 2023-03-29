#include "Serialization/SerializedSystemHandle.h"

namespace DYE::DYEditor
{
	std::optional<std::string> SerializedSystemHandle::TryGetTypeName() const
	{
		auto pTypeNode = m_pSystemTable->get(SystemTypeNameKey);
		if (pTypeNode == nullptr)
		{
			return {};
		}

		return pTypeNode->value<std::string>();
	}

	void SerializedSystemHandle::SetTypeName(std::string const& typeName)
	{
		m_pSystemTable->insert_or_assign(SystemTypeNameKey, typeName);
	}

	std::optional<std::string> SerializedSystemHandle::TryGetGroupName() const
	{
		auto pGroupNode = m_pSystemTable->get(SystemGroupNameKey);
		if (pGroupNode == nullptr)
		{
			return {};
		}

		return pGroupNode->value<std::string>();
	}

	void SerializedSystemHandle::SetGroupName(std::string const& groupName)
	{
		m_pSystemTable->insert_or_assign(SystemGroupNameKey, groupName);
	}

	SerializedSystemHandle::SerializedSystemHandle(toml::table *pSystemTable) : m_pSystemTable(pSystemTable)
	{
	}
}