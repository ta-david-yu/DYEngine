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

	struct SerializedComponent
	{
		friend class SerializedObjectFactory;
		friend class SerializedEntity;

		SerializedComponent() = default;
		
		inline bool IsHandle() const { return m_IsHandle; }
		std::optional<std::string> TryGetTypeName() const;
		void SetTypeName(std::string const& typeName);

		/// \return Return a copy that is not a handle.
		SerializedComponent CloneAsNonHandle() const;

		template<typename T>
		std::optional<T> TryGetPrimitiveTypePropertyValue(std::string_view const& propertyName) const
		{
			toml::table const* pComponentTable = IsHandle() ? m_pComponentTableHandle : &m_ComponentTable;
			toml::node const* pPropertyNode = pComponentTable->get(propertyName);
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
			toml::table* pComponentTable = IsHandle() ? m_pComponentTableHandle : &m_ComponentTable;
			pComponentTable->insert_or_assign(propertyName, value);
		}

	private:
		explicit SerializedComponent(toml::table* pComponentTableHandle);
		explicit SerializedComponent(toml::table&& componentTable);

		// This will only be valid when IsHandle is false
		toml::table m_ComponentTable;

		// This will only be valid when IsHandle is true. In that case it points to the entity table owned by the belonging SerializedScene.
		toml::table* m_pComponentTableHandle = nullptr;

		bool m_IsHandle = false;
	};

	/// We need a GUID specialization because it's stored as string in serialized component.
	template<>
	std::optional<DYE::GUID> SerializedComponent::TryGetPrimitiveTypePropertyValue(std::string_view const& propertyName) const;
	template<>
	std::optional<DYE::Vector2> SerializedComponent::TryGetPrimitiveTypePropertyValue(std::string_view const& propertyName) const;
	template<>
	std::optional<DYE::Vector3> SerializedComponent::TryGetPrimitiveTypePropertyValue(std::string_view const& propertyName) const;
	template<>
	std::optional<DYE::Vector4> SerializedComponent::TryGetPrimitiveTypePropertyValue(std::string_view const& propertyName) const;
	template<>
	std::optional<DYE::Quaternion> SerializedComponent::TryGetPrimitiveTypePropertyValue(std::string_view const& propertyName) const;
	template<>
	std::optional<DYE::AssetPath> SerializedComponent::TryGetPrimitiveTypePropertyValue(std::string_view const& propertyName) const;
	template<>
	std::optional<Math::Rect> SerializedComponent::TryGetPrimitiveTypePropertyValue(std::string_view const& propertyName) const;

	/// We need a GUID specialization because it's stored as string in serialized component.
	template<>
	void SerializedComponent::SetPrimitiveTypePropertyValue<DYE::GUID>(std::string const& propertyName, DYE::GUID const& value);
	template<>
	void SerializedComponent::SetPrimitiveTypePropertyValue<DYE::Vector2>(std::string const& propertyName, DYE::Vector2 const& value);
	template<>
	void SerializedComponent::SetPrimitiveTypePropertyValue<DYE::Vector3>(std::string const& propertyName, DYE::Vector3 const& value);
	template<>
	void SerializedComponent::SetPrimitiveTypePropertyValue<DYE::Vector4>(std::string const& propertyName, DYE::Vector4 const& value);
	template<>
	void SerializedComponent::SetPrimitiveTypePropertyValue<DYE::Quaternion>(std::string const& propertyName, DYE::Quaternion const& value);
	template<>
	void SerializedComponent::SetPrimitiveTypePropertyValue<DYE::AssetPath>(std::string const& propertyName, DYE::AssetPath const& value);
	template<>
	void SerializedComponent::SetPrimitiveTypePropertyValue<Math::Rect>(std::string const& propertyName, Math::Rect const& value);
}