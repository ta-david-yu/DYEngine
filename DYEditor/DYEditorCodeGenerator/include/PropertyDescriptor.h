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

std::string PropertyDescriptorToImGuiUtilControlCallSource(PropertyDescriptor const& descriptor)
{
	std::stringstream stream;

	stream <<
R"(							ImGui::TextWrapped(")" << descriptor.VariableName << " : " << descriptor.TypeSpecifier << R"(");)" << std::endl;

	return stream.str();
	if (descriptor.TypeSpecifier == "Float")
	{

	}
	else if (descriptor.TypeSpecifier == "Bool")
	{

	}
	else if (descriptor.TypeSpecifier == "Vector3")
	{

	}
	else
	{
	}
}