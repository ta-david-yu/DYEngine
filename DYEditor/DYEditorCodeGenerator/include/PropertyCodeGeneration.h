#pragma once

#include <fmt/core.h>

#include <string>

struct PropertyDescriptor
{
	// Parsed from variable declaration
	std::string TypeSpecifier;
	std::string VariableName;
	bool IsConstant;
};

std::string PropertyDescriptorToImGuiUtilControlCallSource(const std::string& componentType, PropertyDescriptor const& propertyDescriptor)
{
	if (propertyDescriptor.IsConstant)
	{
		return fmt::format("\t\t\t\t\t\t\t"
						   "ImGui::BeginDisabled(true); "
						   "ImGuiUtil::DrawReadOnlyTextWithLabel(\"{}\", \"Constant variable of type '{}'\"); "
						   "ImGui::EndDisabled();\n",
						   propertyDescriptor.VariableName, propertyDescriptor.TypeSpecifier);
	}

	if (propertyDescriptor.TypeSpecifier == "Float" ||
		propertyDescriptor.TypeSpecifier == "Double")
	{
		return fmt::format("\t\t\t\t\t\t\tchanged |= ImGuiUtil::DrawFloatControl(\"{}\", entity.GetComponent<{}>().{});\n",
						   propertyDescriptor.VariableName, componentType, propertyDescriptor.VariableName);
	}
	else if (propertyDescriptor.TypeSpecifier == "Bool")
	{
		return fmt::format("\t\t\t\t\t\t\tchanged |= ImGuiUtil::DrawBoolControl(\"{}\", entity.GetComponent<{}>().{});\n",
						   propertyDescriptor.VariableName, componentType, propertyDescriptor.VariableName);
	}
	else if (propertyDescriptor.TypeSpecifier == "String")
	{
		return fmt::format("\t\t\t\t\t\t\tchanged |= ImGuiUtil::DrawTextControl(\"{}\", entity.GetComponent<{}>().{});\n",
						   propertyDescriptor.VariableName, componentType, propertyDescriptor.VariableName);
	}
	else if (propertyDescriptor.TypeSpecifier == "Vector2")
	{
		return fmt::format("\t\t\t\t\t\t\tchanged |= ImGuiUtil::DrawVector2Control(\"{}\", entity.GetComponent<{}>().{});\n",
						   propertyDescriptor.VariableName, componentType, propertyDescriptor.VariableName);
	}
	else if (propertyDescriptor.TypeSpecifier == "Vector3")
	{
		return fmt::format("\t\t\t\t\t\t\tchanged |= ImGuiUtil::DrawVector3Control(\"{}\", entity.GetComponent<{}>().{});\n",
						   propertyDescriptor.VariableName, componentType, propertyDescriptor.VariableName);
	}
	else if (propertyDescriptor.TypeSpecifier == "Color4")
	{
		return fmt::format("\t\t\t\t\t\t\tchanged |= ImGuiUtil::DrawColor4Control(\"{}\", entity.GetComponent<{}>().{});\n",
						   propertyDescriptor.VariableName, componentType, propertyDescriptor.VariableName);
	}
	else if (propertyDescriptor.TypeSpecifier == "Int32" ||
			 propertyDescriptor.TypeSpecifier == "Int64" ||
			 propertyDescriptor.TypeSpecifier == "Int16" ||
			 propertyDescriptor.TypeSpecifier == "Int8")
	{
		return fmt::format("\t\t\t\t\t\t\tchanged |= ImGuiUtil::DrawIntControl(\"{}\", entity.GetComponent<{}>().{});\n",
						   propertyDescriptor.VariableName, componentType, propertyDescriptor.VariableName);
	}
	else if (propertyDescriptor.TypeSpecifier == "Uint32" ||
			 propertyDescriptor.TypeSpecifier == "Uint64" ||
			 propertyDescriptor.TypeSpecifier == "Uint16" ||
			 propertyDescriptor.TypeSpecifier == "Uint8")
	{
		return fmt::format("\t\t\t\t\t\t\tchanged |= ImGuiUtil::DrawIntControl(\"{}\", entity.GetComponent<{}>().{});\n",
						   propertyDescriptor.VariableName, componentType, propertyDescriptor.VariableName);
	}
	else if (propertyDescriptor.TypeSpecifier == "Char")
	{
		return fmt::format("\t\t\t\t\t\t\tchanged |= ImGuiUtil::DrawCharControl(\"{}\", entity.GetComponent<{}>().{});\n",
						   propertyDescriptor.VariableName, componentType, propertyDescriptor.VariableName);
	}
	else if (propertyDescriptor.TypeSpecifier == "Vector4")
	{
		return fmt::format("\t\t\t\t\t\t\tchanged |= ImGuiUtil::DrawVector4Control(\"{}\", entity.GetComponent<{}>().{});\n",
						   propertyDescriptor.VariableName, componentType, propertyDescriptor.VariableName);
	}
	else
	{
		return fmt::format("\t\t\t\t\t\t\t"
						   "ImGui::BeginDisabled(true); "
						   "ImGuiUtil::DrawReadOnlyTextWithLabel(\"{}\", \"Variable of unsupported type '{}'\"); "
						   "ImGui::EndDisabled();\n",
						   propertyDescriptor.VariableName, propertyDescriptor.TypeSpecifier);
	}
}