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
	Quaternion,

	Unsupported
	// TODO: To add more
};

struct PropertyDescriptor
{
	// Parsed from variable declaration
	std::string TypeSpecifierString;
	PropertyType TypeSpecifier;
	std::string VariableName;
	bool HasDefaultValue;
	std::string DefaultValueString;
	bool IsConstant;
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
	else if (typeString == "Quaternion")
	{
		return PropertyType::Quaternion;
	}
	else
	{
		return PropertyType::Unsupported;
	}
}

std::string PropertyDescriptorToDeserializeCallSource(std::string_view const& componentType, PropertyDescriptor const& propertyDescriptor)
{
	if (propertyDescriptor.IsConstant)
	{
		return fmt::format("\t\t\t\t\t\t\t"
						   "// Property '{}' will not be serialized because it is a constant variable.\n",
						   propertyDescriptor.VariableName);
	}

	switch (propertyDescriptor.TypeSpecifier)
	{
		case PropertyType::Unsupported:
			return fmt::format("\t\t\t\t\t\t\t"
							   "// Property '{}' will not be serialized because its type '{}' is not supported.\n",
							   propertyDescriptor.VariableName, propertyDescriptor.TypeSpecifierString);
		case PropertyType::Char:
			// Char needs special handling because some serialization libraries don't support character type.
			if (propertyDescriptor.HasDefaultValue)
			{
				// Because the default value string for a char variable is '0' for instance, index 1 (the 0) will be the default character.
				char defaultValue = propertyDescriptor.DefaultValueString[1];
				return fmt::format("\t\t\t\t\t\t\t"
								   "component.{} = serializedComponent.GetPrimitiveTypePropertyValueOr<const char*>(\"{}\", \"{}\")[0];\n",
								   propertyDescriptor.VariableName, propertyDescriptor.VariableName, defaultValue);
			}
			else
			{
				return fmt::format("\t\t\t\t\t\t\t"
								   "component.{} = serializedComponent.GetPrimitiveTypePropertyValueOr<const char*>(\"{}\", \" \")[0];\n",
								   propertyDescriptor.VariableName, propertyDescriptor.VariableName);
			}
		default:
			if (propertyDescriptor.HasDefaultValue)
			{
				return fmt::format("\t\t\t\t\t\t\t"
								   "component.{} = serializedComponent.GetPrimitiveTypePropertyValueOr<{}>(\"{}\", {});\n",
								   propertyDescriptor.VariableName, propertyDescriptor.TypeSpecifierString,
								   propertyDescriptor.VariableName, propertyDescriptor.DefaultValueString);
			}
			else
			{
				return fmt::format("\t\t\t\t\t\t\t"
								   "component.{} = serializedComponent.GetPrimitiveTypePropertyValueOrDefault<{}>(\"{}\");\n",
								   propertyDescriptor.VariableName, propertyDescriptor.TypeSpecifierString,
								   propertyDescriptor.VariableName);
			}
	}
}

std::string PropertyDescriptorToSerializeCallSource(std::string_view const& componentType, PropertyDescriptor const& propertyDescriptor)
{
	if (propertyDescriptor.IsConstant)
	{
		return fmt::format("\t\t\t\t\t\t\t"
						   "// Property '{}' will not be serialized because it is a constant variable.\n",
						   propertyDescriptor.VariableName);
	}

	switch (propertyDescriptor.TypeSpecifier)
	{
		case PropertyType::Unsupported:
			return fmt::format("\t\t\t\t\t\t\t"
							   "// Property '{}' will not be serialized because its type '{}' is not supported.\n",
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
			return fmt::format("\t\t\t\t\t\t\tchanged |= ImGuiUtil::DrawFloatControl(\"{}\", component.{});\n",
							   propertyDescriptor.VariableName, propertyDescriptor.VariableName);
		case PropertyType::Bool:
			return fmt::format("\t\t\t\t\t\t\tchanged |= ImGuiUtil::DrawBoolControl(\"{}\", component.{});\n",
							   propertyDescriptor.VariableName, propertyDescriptor.VariableName);
		case PropertyType::String:
			return fmt::format("\t\t\t\t\t\t\tchanged |= ImGuiUtil::DrawTextControl(\"{}\", component.{});\n",
							   propertyDescriptor.VariableName, propertyDescriptor.VariableName);
		case PropertyType::Vector2:
			return fmt::format("\t\t\t\t\t\t\tchanged |= ImGuiUtil::DrawVector2Control(\"{}\", component.{});\n",
							   propertyDescriptor.VariableName, propertyDescriptor.VariableName);
		case PropertyType::Vector3:
			return fmt::format("\t\t\t\t\t\t\tchanged |= ImGuiUtil::DrawVector3Control(\"{}\", component.{});\n",
							   propertyDescriptor.VariableName, propertyDescriptor.VariableName);
		case PropertyType::Color4:
			return fmt::format("\t\t\t\t\t\t\tchanged |= ImGuiUtil::DrawColor4Control(\"{}\", component.{});\n",
							   propertyDescriptor.VariableName, propertyDescriptor.VariableName);
		case PropertyType::Int32:
		case PropertyType::Int64:
		case PropertyType::Int16:
		case PropertyType::Int8:
			return fmt::format("\t\t\t\t\t\t\tchanged |= ImGuiUtil::DrawIntControl(\"{}\", component.{});\n",
							   propertyDescriptor.VariableName, propertyDescriptor.VariableName);
		case PropertyType::Uint32:
		case PropertyType::Uint64:
		case PropertyType::Uint16:
		case PropertyType::Uint8:
			return fmt::format("\t\t\t\t\t\t\tchanged |= ImGuiUtil::DrawIntControl(\"{}\", component.{});\n",
							   propertyDescriptor.VariableName, propertyDescriptor.VariableName);
		case PropertyType::Char:
			return fmt::format("\t\t\t\t\t\t\tchanged |= ImGuiUtil::DrawCharControl(\"{}\", component.{});\n",
							   propertyDescriptor.VariableName, propertyDescriptor.VariableName);
		case PropertyType::Vector4:
			return fmt::format("\t\t\t\t\t\t\tchanged |= ImGuiUtil::DrawVector4Control(\"{}\", component.{});\n",
							   propertyDescriptor.VariableName, propertyDescriptor.VariableName);
		case PropertyType::Quaternion:
			return fmt::format("\t\t\t\t\t\t\t// '{}' : Quaternion \n"
							   "\t\t\t\t\t\t\t{}\n"
							   "\t\t\t\t\t\t\t\tglm::vec3 eulerDegree = glm::eulerAngles(component.{});\n"
							   "\t\t\t\t\t\t\t\teulerDegree += glm::vec3(0.f); eulerDegree = glm::degrees(eulerDegree);\n"
							   "\t\t\t\t\t\t\t\tif (ImGuiUtil::DrawVector3Control(\"{}\", eulerDegree))\n"
							   "\t\t\t\t\t\t\t\t{}\n"		// THIS is causing the error!! Have to use the hack.
							   "\t\t\t\t\t\t\t\t	eulerDegree.y = glm::clamp(eulerDegree.y, -90.f, 90.f);\n"
							   "\t\t\t\t\t\t\t\t	component.{} = glm::quat (glm::radians(eulerDegree));\n"
							   "\t\t\t\t\t\t\t\t	changed = true;\n"
							   "\t\t\t\t\t\t\t\t{}\n"
							   "\t\t\t\t\t\t\t{}\n", 	// THIS is causing the error!! Have to use the hack.
							   propertyDescriptor.VariableName, "{", propertyDescriptor.VariableName, propertyDescriptor.VariableName, "{", propertyDescriptor.VariableName, "}", "}");
		case PropertyType::Unsupported:
		default:
			return fmt::format("\t\t\t\t\t\t\t"
							   "ImGui::BeginDisabled(true); "
							   "ImGuiUtil::DrawReadOnlyTextWithLabel(\"{}\", \"Variable of unsupported type '{}'\"); "
							   "ImGui::EndDisabled();\n",
							   propertyDescriptor.VariableName, propertyDescriptor.TypeSpecifierString);
	}
}