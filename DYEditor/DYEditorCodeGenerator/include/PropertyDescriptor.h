#pragma once

#include <string>
#include <sstream>

struct PropertyDescriptor
{
	// Parsed from variable declaration
	std::string TypeSpecifier;
	std::string VariableName;
	bool IsConstant;
};

std::string PropertyDescriptorToImGuiUtilControlCallSource(std::string componentType, PropertyDescriptor const& propertyDescriptor)
{
	std::stringstream stream;

	if (propertyDescriptor.IsConstant)
	{
		stream <<
			   R"(							ImGui::TextWrapped(")" << propertyDescriptor.VariableName << " : const " << propertyDescriptor.TypeSpecifier << R"(");)" << std::endl;
		return stream.str();
	}

	if (propertyDescriptor.TypeSpecifier == "Float")
	{
		stream 	<< "\t\t\t\t\t\t\tchanged |= ImGuiUtil::DrawFloatControl(\""
				<< propertyDescriptor.VariableName << "\", "
				<< "entity.GetComponent<" << componentType << ">()."
				<< propertyDescriptor.VariableName << ");\n";
	}
	else if (propertyDescriptor.TypeSpecifier == "Int32")
	{
		stream 	<< "\t\t\t\t\t\t\tchanged |= ImGuiUtil::DrawIntControl(\""
				  << propertyDescriptor.VariableName << "\", "
				  << "entity.GetComponent<" << componentType << ">()."
				  << propertyDescriptor.VariableName << ");\n";
	}
	else if (propertyDescriptor.TypeSpecifier == "Bool")
	{
		stream 	<< "\t\t\t\t\t\t\tchanged |= ImGuiUtil::DrawBoolControl(\""
				  << propertyDescriptor.VariableName << "\", "
				  << "entity.GetComponent<" << componentType << ">()."
				  << propertyDescriptor.VariableName << ");\n";
	}
	else
	{
		stream <<
			   R"(							ImGui::TextWrapped(")" << propertyDescriptor.VariableName << " : " << propertyDescriptor.TypeSpecifier << R"(");)" << std::endl;
	}

	return stream.str();
}