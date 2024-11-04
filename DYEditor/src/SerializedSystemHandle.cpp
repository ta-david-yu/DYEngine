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

    void SerializedSystemHandle::SetTypeName(std::string const &typeName)
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

    void SerializedSystemHandle::SetGroupName(std::string const &groupName)
    {
        m_pSystemTable->insert_or_assign(SystemGroupNameKey, groupName);
    }

    bool SerializedSystemHandle::GetIsEnabledOr(bool defaultValue) const
    {
        auto pIsEnabledNode = m_pSystemTable->get(IsSystemEnabledKey);
        if (pIsEnabledNode == nullptr)
        {
            return defaultValue;
        }

        auto tryGetIsEnabledResult = pIsEnabledNode->value<bool>();
        if (!tryGetIsEnabledResult.has_value())
        {
            return defaultValue;
        }

        return tryGetIsEnabledResult.value();
    }

    void SerializedSystemHandle::SetIsEnabled(bool value)
    {
        m_pSystemTable->insert_or_assign(IsSystemEnabledKey, value);
    }

    SerializedSystemHandle::SerializedSystemHandle(toml::table *pSystemTable) : m_pSystemTable(pSystemTable)
    {
    }
}