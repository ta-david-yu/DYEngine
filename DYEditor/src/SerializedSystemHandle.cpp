#include "Serialization/SerializedSystemHandle.h"

namespace DYE::DYEditor
{
	std::optional<std::string> SerializedSystemHandle::GetTypeName() const
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

	SerializedSystemHandle::SerializedSystemHandle(toml::table *pSystemTable) : m_pSystemTable(pSystemTable)
	{
	}
}