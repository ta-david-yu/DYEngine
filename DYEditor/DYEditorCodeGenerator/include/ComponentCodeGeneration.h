#pragma once

#include <fmt/core.h>

#include <regex>

struct ComponentDescriptor
{
	std::string LocatedHeaderFile;
	std::string CustomName;
	std::string FullType;
	std::vector<PropertyDescriptor> Properties;
};

char const *ComponentTypeRegistrationCallSourceStart =
	R"(		TypeRegistry::RegisterComponentType<${COMPONENT_FULL_TYPE}>
			(
				"${COMPONENT_NAME}",
				ComponentTypeFunctionCollection
					{
						.DrawInspector = [](Entity &entity)
						{
							bool changed = false;
							ImGui::TextWrapped("${COMPONENT_FULL_TYPE}");
)";

char const* ComponentTypeRegistrationCallSourceEnd =
	R"(							return changed;
						}
					}
			);

)";

std::string ComponentDescriptorToTypeRegistrationCallSource(ComponentDescriptor const& descriptor)
{
	auto const& componentName = descriptor.CustomName;
	auto const& componentFullType = descriptor.FullType;

	std::string result = "\t\t// Component located in " + descriptor.LocatedHeaderFile + "\n";
	result.append(ComponentTypeRegistrationCallSourceStart);

	for (auto const& propertyDescriptor : descriptor.Properties)
	{
		result.append(PropertyDescriptorToImGuiUtilControlCallSource(descriptor.FullType, propertyDescriptor));
	}

	result.append(ComponentTypeRegistrationCallSourceEnd);

	std::regex const componentNameKeywordPattern(R"(\$\{COMPONENT_NAME\})");
	std::regex const componentFullTypeKeywordPattern(R"(\$\{COMPONENT_FULL_TYPE\})");

	result = std::regex_replace(result, componentNameKeywordPattern, componentName);
	result = std::regex_replace(result, componentFullTypeKeywordPattern, componentFullType);

	return result;
}