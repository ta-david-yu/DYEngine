#pragma once

#include "Core/EditorCore.h"
#include "Core/EditorProperty.h"
#include "Math/Rect.h"

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
		T GetPrimitiveTypePropertyValueOrDefault(std::string_view const& propertyName) const
		{
			auto getValueResult = TryGetPrimitiveTypePropertyValue<T>(propertyName);
			if (!getValueResult.has_value())
			{
				return T();
			}

			return getValueResult.value();
		}

		template<typename T>
		T GetPrimitiveTypePropertyValueOr(std::string_view const& propertyName, T const& defaultValue) const
		{
			auto getValueResult = TryGetPrimitiveTypePropertyValue<T>(propertyName);
			if (!getValueResult.has_value())
			{
				return defaultValue;
			}

			return getValueResult.value();
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

	template<>
	std::optional<DYE::Vector2> SerializedComponentHandle::TryGetPrimitiveTypePropertyValue(std::string_view const& propertyName) const;
	template<>
	std::optional<DYE::Vector3> SerializedComponentHandle::TryGetPrimitiveTypePropertyValue(std::string_view const& propertyName) const;
	template<>
	std::optional<DYE::Vector4> SerializedComponentHandle::TryGetPrimitiveTypePropertyValue(std::string_view const& propertyName) const;
	template<>
	std::optional<DYE::Quaternion> SerializedComponentHandle::TryGetPrimitiveTypePropertyValue(std::string_view const& propertyName) const;
	template<>
	std::optional<DYE::AssetPath> SerializedComponentHandle::TryGetPrimitiveTypePropertyValue(std::string_view const& propertyName) const;
	template<>
	std::optional<Math::Rect> SerializedComponentHandle::TryGetPrimitiveTypePropertyValue(std::string_view const& propertyName) const;

	template<>
	void SerializedComponentHandle::SetPrimitiveTypePropertyValue<DYE::Vector2>(std::string const& propertyName, DYE::Vector2 const& value);
	template<>
	void SerializedComponentHandle::SetPrimitiveTypePropertyValue<DYE::Vector3>(std::string const& propertyName, DYE::Vector3 const& value);
	template<>
	void SerializedComponentHandle::SetPrimitiveTypePropertyValue<DYE::Vector4>(std::string const& propertyName, DYE::Vector4 const& value);
	template<>
	void SerializedComponentHandle::SetPrimitiveTypePropertyValue<DYE::Quaternion>(std::string const& propertyName, DYE::Quaternion const& value);
	template<>
	void SerializedComponentHandle::SetPrimitiveTypePropertyValue<DYE::AssetPath>(std::string const& propertyName, DYE::AssetPath const& value);
	template<>
	void SerializedComponentHandle::SetPrimitiveTypePropertyValue<Math::Rect>(std::string const& propertyName, Math::Rect const& value);
}