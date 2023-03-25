#pragma once

#include <fmt/core.h>

#include <string>

enum class PropertyType
{
	Float,
	Double,
	Bool,
	String,
	Vector2,
	Vector3,
	Color4,
	Int32,
	Int64,
	Int16,
	Int8,
	Uint32,
	Uint64,
	Uint16,
	Uint8,
	Char,
	Vector4,

	Unsupported
	// TODO: To add more
};

struct PropertyDescriptor
{
	// Parsed from variable declaration
	std::string TypeSpecifierString;
	std::string VariableName;
	bool IsConstant;
	PropertyType TypeSpecifier;
};

PropertyType CastTypeSpecifierStringToPropertyType(std::string_view const& typeString)
{
	if (typeString == "Float")
	{
		return PropertyType::Float;
	}
	else if (typeString == "Double")
	{
		return PropertyType::Double;
	}
	else if (typeString == "Bool")
	{
		return PropertyType::Bool;
	}
	else if (typeString == "String")
	{
		return PropertyType::String;
	}
	else if (typeString == "Vector2")
	{
		return PropertyType::Vector2;
	}
	else if (typeString == "Vector3")
	{
		return PropertyType::Vector3;
	}
	else if (typeString == "Color4")
	{
		return PropertyType::Color4;
	}
	else if (typeString == "Int32")
	{
		return PropertyType::Int32;
	}
	else if (typeString == "Int64")
	{
		return PropertyType::Int64;
	}
	else if (typeString == "Int16")
	{
		return PropertyType::Int16;
	}
	else if (typeString == "Int8")
	{
		return PropertyType::Int8;
	}
	else if (typeString == "Uint32")
	{
		return PropertyType::Uint32;
	}
	else if (typeString == "Uint64")
	{
		return PropertyType::Uint64;
	}
	else if (typeString == "Uint16")
	{
		return PropertyType::Uint16;
	}
	else if (typeString == "Uint8")
	{
		return PropertyType::Uint8;
	}
	else if (typeString == "Char")
	{
		return PropertyType::Char;
	}
	else if (typeString == "Vector4")
	{
		return PropertyType::Vector4;
	}
	else
	{
		return PropertyType::Unsupported;
	}
}

std::string PropertyDescriptorToSerializeCallSource(std::string_view const& componentType, PropertyDescriptor const& propertyDescriptor)
{
	if (propertyDescriptor.IsConstant)
	{
		return fmt::format("\t\t\t\t\t\t\t"
			   				"// {} will not be serialized because it is a constant variable.\n", propertyDescriptor.VariableName);
	}

	if (propertyDescriptor.TypeSpecifier == PropertyType::Unsupported)
	{
		return fmt::format("\t\t\t\t\t\t\t"
						   "// {} will not be serialized because its type {} is not supported.\n", propertyDescriptor.VariableName, propertyDescriptor.TypeSpecifierString);
	}

	switch (propertyDescriptor.TypeSpecifier)
	{
		case PropertyType::Unsupported:
			return fmt::format("\t\t\t\t\t\t\t"
							   "// {} will not be serialized because its type {} is not supported.\n",
							   propertyDescriptor.VariableName, propertyDescriptor.TypeSpecifierString);
		case PropertyType::Char:
			// Char needs special handling because some serialization libraries don't support character type.
			return fmt::format("\t\t\t\t\t\t\t"
							   "std::string {}(\" \"); {}[0] = component.{}; serializedComponent.SetPrimitiveTypePropertyValue(\"{}\", {});",
							   propertyDescriptor.VariableName, propertyDescriptor.VariableName, propertyDescriptor.VariableName,
							   propertyDescriptor.VariableName, propertyDescriptor.VariableName);
		default:
			return fmt::format("\t\t\t\t\t\t\t"
							   "serializedComponent.SetPrimitiveTypePropertyValue(\"{}\", component.{});\n",
							   propertyDescriptor.VariableName, propertyDescriptor.VariableName);
	}
}

std::string PropertyDescriptorToImGuiUtilControlCallSource(std::string_view const& componentType, PropertyDescriptor const& propertyDescriptor)
{
	if (propertyDescriptor.IsConstant)
	{
		return fmt::format("\t\t\t\t\t\t\t"
						   "ImGui::BeginDisabled(true); "
						   "ImGuiUtil::DrawReadOnlyTextWithLabel(\"{}\", \"Constant variable of type '{}'\"); "
						   "ImGui::EndDisabled();\n",
						   propertyDescriptor.VariableName, propertyDescriptor.TypeSpecifierString);
	}

	switch (propertyDescriptor.TypeSpecifier)
	{
		case PropertyType::Float:
		case PropertyType::Double:
			return fmt::format("\t\t\t\t\t\t\tchanged |= ImGuiUtil::DrawFloatControl(\"{}\", entity.GetComponent<{}>().{});\n",
							   propertyDescriptor.VariableName, componentType, propertyDescriptor.VariableName);
		case PropertyType::Bool:
			return fmt::format("\t\t\t\t\t\t\tchanged |= ImGuiUtil::DrawBoolControl(\"{}\", entity.GetComponent<{}>().{});\n",
							   propertyDescriptor.VariableName, componentType, propertyDescriptor.VariableName);
		case PropertyType::String:
			return fmt::format("\t\t\t\t\t\t\tchanged |= ImGuiUtil::DrawTextControl(\"{}\", entity.GetComponent<{}>().{});\n",
							   propertyDescriptor.VariableName, componentType, propertyDescriptor.VariableName);
		case PropertyType::Vector2:
			return fmt::format("\t\t\t\t\t\t\tchanged |= ImGuiUtil::DrawVector2Control(\"{}\", entity.GetComponent<{}>().{});\n",
							   propertyDescriptor.VariableName, componentType, propertyDescriptor.VariableName);
		case PropertyType::Vector3:
			return fmt::format("\t\t\t\t\t\t\tchanged |= ImGuiUtil::DrawVector3Control(\"{}\", entity.GetComponent<{}>().{});\n",
							   propertyDescriptor.VariableName, componentType, propertyDescriptor.VariableName);
		case PropertyType::Color4:
			return fmt::format("\t\t\t\t\t\t\tchanged |= ImGuiUtil::DrawColor4Control(\"{}\", entity.GetComponent<{}>().{});\n",
							   propertyDescriptor.VariableName, componentType, propertyDescriptor.VariableName);
		case PropertyType::Int32:
		case PropertyType::Int64:
		case PropertyType::Int16:
		case PropertyType::Int8:
			return fmt::format("\t\t\t\t\t\t\tchanged |= ImGuiUtil::DrawIntControl(\"{}\", entity.GetComponent<{}>().{});\n",
							   propertyDescriptor.VariableName, componentType, propertyDescriptor.VariableName);
		case PropertyType::Uint32:
		case PropertyType::Uint64:
		case PropertyType::Uint16:
		case PropertyType::Uint8:
			return fmt::format("\t\t\t\t\t\t\tchanged |= ImGuiUtil::DrawIntControl(\"{}\", entity.GetComponent<{}>().{});\n",
							   propertyDescriptor.VariableName, componentType, propertyDescriptor.VariableName);
		case PropertyType::Char:
			return fmt::format("\t\t\t\t\t\t\tchanged |= ImGuiUtil::DrawCharControl(\"{}\", entity.GetComponent<{}>().{});\n",
							   propertyDescriptor.VariableName, componentType, propertyDescriptor.VariableName);
		case PropertyType::Vector4:
			return fmt::format("\t\t\t\t\t\t\tchanged |= ImGuiUtil::DrawVector4Control(\"{}\", entity.GetComponent<{}>().{});\n",
							   propertyDescriptor.VariableName, componentType, propertyDescriptor.VariableName);
		case PropertyType::Unsupported:
		default:
			return fmt::format("\t\t\t\t\t\t\t"
							   "ImGui::BeginDisabled(true); "
							   "ImGuiUtil::DrawReadOnlyTextWithLabel(\"{}\", \"Variable of unsupported type '{}'\"); "
							   "ImGui::EndDisabled();\n",
							   propertyDescriptor.VariableName, propertyDescriptor.TypeSpecifierString);
	}
}