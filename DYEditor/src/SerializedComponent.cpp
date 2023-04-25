#include "Serialization/SerializedComponent.h"
#include "Core/EditorProperty.h"

namespace DYE::DYEditor
{
	std::optional<std::string> SerializedComponent::TryGetTypeName() const
	{
		toml::table const* pComponentTable = IsHandle() ? m_pComponentTableHandle : &m_ComponentTable;
		auto pTypeNode = pComponentTable->get(ComponentTypeNameKey);
		if (pTypeNode == nullptr)
		{
			return {};
		}

		return pTypeNode->value<std::string>();
	}

	void SerializedComponent::SetTypeName(std::string const& typeName)
	{
		toml::table* pComponentTable = IsHandle() ? m_pComponentTableHandle : &m_ComponentTable;
		pComponentTable->insert_or_assign(ComponentTypeNameKey, typeName);
	}

	SerializedComponent::SerializedComponent(toml::table *pComponentTableHandle) : m_pComponentTableHandle(pComponentTableHandle), m_IsHandle(true)
	{
	}

	SerializedComponent::SerializedComponent(toml::table&& componentTable) : m_ComponentTable(componentTable), m_IsHandle(false)
	{

	}

	template<>
	std::optional<DYE::GUID> SerializedComponent::TryGetPrimitiveTypePropertyValue(std::string_view const& propertyName) const
	{
		toml::table const* pComponentTable = IsHandle() ? m_pComponentTableHandle : &m_ComponentTable;
		toml::node const* pPropertyNode = pComponentTable->get(propertyName);
		if (pPropertyNode == nullptr)
		{
			return {};
		}

		auto tryGetGUIDString = pPropertyNode->value<std::string>();
		if (!tryGetGUIDString.has_value())
		{
			return {};
		}

		return (DYE::GUID) std::stoull(tryGetGUIDString.value());
	}

	template<>
	std::optional<DYE::Vector2> SerializedComponent::TryGetPrimitiveTypePropertyValue(std::string_view const& propertyName) const
	{
		toml::table const* pComponentTable = IsHandle() ? m_pComponentTableHandle : &m_ComponentTable;
		toml::node const* pPropertyNode = pComponentTable->get(propertyName);
		if (pPropertyNode == nullptr)
		{
			return {};
		}

		toml::table const* pPropertyTable = pPropertyNode->as_table();
		auto x = pPropertyTable->get("x")->value_or<float>(0);
		auto y = pPropertyTable->get("y")->value_or<float>(0);
		return DYE::Vector2 { x, y };
	}

	template<>
	std::optional<DYE::Vector3> SerializedComponent::TryGetPrimitiveTypePropertyValue(std::string_view const& propertyName) const
	{
		toml::table const* pComponentTable = IsHandle() ? m_pComponentTableHandle : &m_ComponentTable;
		toml::node const* pPropertyNode = pComponentTable->get(propertyName);
		if (pPropertyNode == nullptr)
		{
			return {};
		}

		toml::table const* pPropertyTable = pPropertyNode->as_table();
		auto x = pPropertyTable->get("x")->value_or<float>(0);
		auto y = pPropertyTable->get("y")->value_or<float>(0);
		auto z = pPropertyTable->get("z")->value_or<float>(0);
		return DYE::Vector3 { x, y, z };
	}

	template<>
	std::optional<DYE::Vector4> SerializedComponent::TryGetPrimitiveTypePropertyValue(std::string_view const& propertyName) const
	{
		toml::table const* pComponentTable = IsHandle() ? m_pComponentTableHandle : &m_ComponentTable;
		toml::node const* pPropertyNode = pComponentTable->get(propertyName);
		if (pPropertyNode == nullptr)
		{
			return {};
		}

		toml::table const* pPropertyTable = pPropertyNode->as_table();
		auto x = pPropertyTable->get("x")->value_or<float>(0);
		auto y = pPropertyTable->get("y")->value_or<float>(0);
		auto z = pPropertyTable->get("z")->value_or<float>(0);
		auto w = pPropertyTable->get("w")->value_or<float>(0);
		return DYE::Vector4 { x, y, z, w };
	}

	template<>
	std::optional<DYE::Quaternion> SerializedComponent::TryGetPrimitiveTypePropertyValue(std::string_view const& propertyName) const
	{
		toml::table const* pComponentTable = IsHandle() ? m_pComponentTableHandle : &m_ComponentTable;
		toml::node const* pPropertyNode = pComponentTable->get(propertyName);
		if (pPropertyNode == nullptr)
		{
			return {};
		}

		toml::table const* pPropertyTable = pPropertyNode->as_table();
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
	std::optional<DYE::AssetPath> SerializedComponent::TryGetPrimitiveTypePropertyValue(std::string_view const& propertyName) const
	{
		toml::table const* pComponentTable = IsHandle() ? m_pComponentTableHandle : &m_ComponentTable;
		toml::node const* pPropertyNode = pComponentTable->get(propertyName);
		if (pPropertyNode == nullptr)
		{
			return {};
		}

		return pPropertyNode->value<DYE::String>();
	}

	template<>
	std::optional<Math::Rect> SerializedComponent::TryGetPrimitiveTypePropertyValue(std::string_view const& propertyName) const
	{
		toml::table const* pComponentTable = IsHandle() ? m_pComponentTableHandle : &m_ComponentTable;
		toml::node const* pPropertyNode = pComponentTable->get(propertyName);
		if (pPropertyNode == nullptr)
		{
			return {};
		}

		toml::table const* pPropertyTable = pPropertyNode->as_table();
		auto x = pPropertyTable->get("X")->value_or<float>(0);
		auto y = pPropertyTable->get("Y")->value_or<float>(0);
		auto width = pPropertyTable->get("Width")->value_or<float>(0);
		auto height = pPropertyTable->get("Height")->value_or<float>(0);
		return Math::Rect(x, y, width, height);
	}

	template<>
	void SerializedComponent::SetPrimitiveTypePropertyValue<DYE::GUID>(std::string const& propertyName, DYE::GUID const& value)
	{
		toml::table* pComponentTable = IsHandle() ? m_pComponentTableHandle : &m_ComponentTable;
		pComponentTable->insert_or_assign(propertyName, std::to_string((std::uint64_t) value));
	}

	template<>
	void SerializedComponent::SetPrimitiveTypePropertyValue<DYE::Vector2>(std::string const& propertyName, DYE::Vector2 const& value)
	{
		toml::table table { {"x", value.x}, {"y", value.y} };
		table.is_inline(true);
		toml::table* pComponentTable = IsHandle() ? m_pComponentTableHandle : &m_ComponentTable;
		pComponentTable->insert_or_assign(propertyName, std::move(table));
	}

	template<>
	void SerializedComponent::SetPrimitiveTypePropertyValue<DYE::Vector3>(std::string const& propertyName, DYE::Vector3 const& value)
	{
		toml::table table { {"x", value.x}, {"y", value.y}, {"z", value.z} };
		table.is_inline(true);
		toml::table* pComponentTable = IsHandle() ? m_pComponentTableHandle : &m_ComponentTable;
		pComponentTable->insert_or_assign(propertyName, std::move(table));
	}

	template<>
	void SerializedComponent::SetPrimitiveTypePropertyValue<DYE::Vector4>(std::string const& propertyName, DYE::Vector4 const& value)
	{
		toml::table table { {"x", value.x}, {"y", value.y}, {"z", value.z}, {"w", value.w} };
		table.is_inline(true);
		toml::table* pComponentTable = IsHandle() ? m_pComponentTableHandle : &m_ComponentTable;
		pComponentTable->insert_or_assign(propertyName, std::move(table));
	}

	template<>
	void SerializedComponent::SetPrimitiveTypePropertyValue<DYE::Quaternion>(std::string const& propertyName, DYE::Quaternion const& value)
	{
		toml::table table { {"x", value.x}, {"y", value.y}, {"z", value.z}, {"w", value.w} };
		table.is_inline(true);
		toml::table* pComponentTable = IsHandle() ? m_pComponentTableHandle : &m_ComponentTable;
		pComponentTable->insert_or_assign(propertyName, std::move(table));
	}

	template<>
	void SerializedComponent::SetPrimitiveTypePropertyValue<DYE::AssetPath>(std::string const& propertyName, DYE::AssetPath const& value)
	{
		toml::table* pComponentTable = IsHandle() ? m_pComponentTableHandle : &m_ComponentTable;
		pComponentTable->insert_or_assign(propertyName, value.string());
	}

	template<>
	void SerializedComponent::SetPrimitiveTypePropertyValue<Math::Rect>(std::string const& propertyName, Math::Rect const& value)
	{
		toml::table table { {"X", value.X}, {"Y", value.Y}, {"Width", value.Width}, {"Height", value.Height} };
		table.is_inline(true);
		toml::table* pComponentTable = IsHandle() ? m_pComponentTableHandle : &m_ComponentTable;
		pComponentTable->insert_or_assign(propertyName, table);
	}
}