#pragma once

#include "EditorCore.h"

#include <string>
#include <optional>

#include <toml++/toml.h>

namespace DYE::DYEditor
{
	constexpr const char* ComponentTypeNameKey = "Type";

	struct SerializedComponentHandle
	{
		friend class SerializedEntity;
		SerializedComponentHandle() = delete;

		std::optional<std::string> TryGetTypeName() const;
		void SetTypeName(std::string const& typeName);

		template<typename T>
		std::optional<T> TryGetPrimitiveTypePropertyValue(std::string_view const& propertyName) const
		{
			toml::node* pPropertyNode = m_pComponentTable->get(propertyName);
			if (pPropertyNode == nullptr)
			{
				return {};
			}

			return pPropertyNode->value<T>();
		}

		template<typename T>
		void SetPrimitiveTypePropertyValue(std::string const& propertyName, T const& value)
		{
			m_pComponentTable->insert_or_assign(propertyName, value);
		}

	private:
		explicit SerializedComponentHandle(toml::table* pComponentTable);
		toml::table* m_pComponentTable = nullptr;
	};
}