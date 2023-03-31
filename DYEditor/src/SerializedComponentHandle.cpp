#include "Serialization/SerializedComponentHandle.h"
#include "Core/EditorProperty.h"

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

	template<>
	std::optional<DYE::Vector2> SerializedComponentHandle::TryGetPrimitiveTypePropertyValue(std::string_view const& propertyName) const
	{
		toml::node* pPropertyNode = m_pComponentTable->get(propertyName);
		if (pPropertyNode == nullptr)
		{
			return {};
		}

		toml::table* pPropertyTable = pPropertyNode->as_table();
		auto x = pPropertyTable->get("x")->value_or<float>(0);
		auto y = pPropertyTable->get("y")->value_or<float>(0);
		return DYE::Vector2 { x, y };
	}

	template<>
	std::optional<DYE::Vector3> SerializedComponentHandle::TryGetPrimitiveTypePropertyValue(std::string_view const& propertyName) const
	{
		toml::node* pPropertyNode = m_pComponentTable->get(propertyName);
		if (pPropertyNode == nullptr)
		{
			return {};
		}

		toml::table* pPropertyTable = pPropertyNode->as_table();
		auto x = pPropertyTable->get("x")->value_or<float>(0);
		auto y = pPropertyTable->get("y")->value_or<float>(0);
		auto z = pPropertyTable->get("z")->value_or<float>(0);
		return DYE::Vector3 { x, y, z };
	}

	template<>
	std::optional<DYE::Vector4> SerializedComponentHandle::TryGetPrimitiveTypePropertyValue(std::string_view const& propertyName) const
	{
		toml::node* pPropertyNode = m_pComponentTable->get(propertyName);
		if (pPropertyNode == nullptr)
		{
			return {};
		}

		toml::table* pPropertyTable = pPropertyNode->as_table();
		auto x = pPropertyTable->get("x")->value_or<float>(0);
		auto y = pPropertyTable->get("y")->value_or<float>(0);
		auto z = pPropertyTable->get("z")->value_or<float>(0);
		auto w = pPropertyTable->get("w")->value_or<float>(0);
		return DYE::Vector4 { x, y, z, w };
	}

	template<>
	std::optional<DYE::Quaternion> SerializedComponentHandle::TryGetPrimitiveTypePropertyValue(std::string_view const& propertyName) const
	{
		toml::node* pPropertyNode = m_pComponentTable->get(propertyName);
		if (pPropertyNode == nullptr)
		{
			return {};
		}

		toml::table* pPropertyTable = pPropertyNode->as_table();
		auto x = pPropertyTable->get("x")->value_or<float>(0);
		auto y = pPropertyTable->get("y")->value_or<float>(0);
		auto z = pPropertyTable->get("z")->value_or<float>(0);
		auto w = pPropertyTable->get("w")->value_or<float>(0);
#ifdef GLM_FORCE_QUAT_DATA_XYZW
		return DYE::Quaternion { x, y, z, w };
#else
		return DYE::Quaternion { w, x, y, z };
#endif
	}

	template<>
	std::optional<DYE::FilePath> SerializedComponentHandle::TryGetPrimitiveTypePropertyValue(std::string_view const& propertyName) const
	{
		toml::node* pPropertyNode = m_pComponentTable->get(propertyName);
		if (pPropertyNode == nullptr)
		{
			return {};
		}

		return pPropertyNode->value<DYE::String>();
	}


	template<>
	void SerializedComponentHandle::SetPrimitiveTypePropertyValue<DYE::Vector2>(std::string const& propertyName, DYE::Vector2 const& value)
	{
		toml::table table { {"x", value.x}, {"y", value.y} };
		table.is_inline(true);
		m_pComponentTable->insert_or_assign(propertyName, std::move(table));
	}

	template<>
	void SerializedComponentHandle::SetPrimitiveTypePropertyValue<DYE::Vector3>(std::string const& propertyName, DYE::Vector3 const& value)
	{
		toml::table table { {"x", value.x}, {"y", value.y}, {"z", value.z} };
		table.is_inline(true);
		m_pComponentTable->insert_or_assign(propertyName, std::move(table));
	}

	template<>
	void SerializedComponentHandle::SetPrimitiveTypePropertyValue<DYE::Vector4>(std::string const& propertyName, DYE::Vector4 const& value)
	{
		toml::table table { {"x", value.x}, {"y", value.y}, {"z", value.z}, {"w", value.w} };
		table.is_inline(true);
		m_pComponentTable->insert_or_assign(propertyName, std::move(table));
	}

	template<>
	void SerializedComponentHandle::SetPrimitiveTypePropertyValue<DYE::Quaternion>(std::string const& propertyName, DYE::Quaternion const& value)
	{
		toml::table table { {"x", value.x}, {"y", value.y}, {"z", value.z}, {"w", value.w} };
		table.is_inline(true);
		m_pComponentTable->insert_or_assign(propertyName, std::move(table));
	}

	template<>
	void SerializedComponentHandle::SetPrimitiveTypePropertyValue<DYE::FilePath>(std::string const& propertyName, DYE::FilePath const& value)
	{
		m_pComponentTable->insert_or_assign(propertyName, value.string());
	}
}