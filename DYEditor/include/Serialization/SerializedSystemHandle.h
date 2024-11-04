#pragma once

#include <string>
#include <optional>
#include <toml++/toml.h>

namespace DYE::DYEditor
{
    constexpr const char *SystemTypeNameKey = "Type";
    constexpr const char *SystemGroupNameKey = "Group";
    constexpr const char *IsSystemEnabledKey = "IsEnabled";

    struct SerializedSystemHandle
    {
        friend class SerializedScene;

        SerializedSystemHandle() = delete;

        std::optional<std::string> TryGetTypeName() const;
        void SetTypeName(std::string const &typeName);

        std::optional<std::string> TryGetGroupName() const;
        void SetGroupName(std::string const &groupName);

        bool GetIsEnabledOr(bool defaultValue) const;
        void SetIsEnabled(bool value);

    private:
        explicit SerializedSystemHandle(toml::table *pSystemTable);
        toml::table *m_pSystemTable = nullptr;
    };
}