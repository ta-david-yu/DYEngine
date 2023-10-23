#pragma once

#include <fmt/core.h>

struct SystemDescriptor
{
	std::string LocatedHeaderFile;
	std::string FullType;
	std::string CustomName;
};

char const *SystemRegistrationCallSource =
	R"(		static ${SYSTEM_FULL_TYPE} _${SYSTEM_TYPE};
		TypeRegistry::RegisterSystem("${SYSTEM_NAME}", &_${SYSTEM_TYPE});

)";

std::string SystemDescriptorToTypeRegistrationCallSource(SystemDescriptor const& descriptor)
{
	auto const& systemName = descriptor.CustomName;
	auto const& systemFullType = descriptor.FullType;

	std::regex const fullTypeNamePattern(R"(^((?:\w+::)*)(\w+)$)");
	std::smatch match;	std::regex_match(systemFullType, match, fullTypeNamePattern);

	auto const& systemTypeNamespacePrefix = match[1].str();	// Namespace(s) with the last '::'
	auto const& systemType = match[2].str();				// Typename without namespaces

	std::string result = "\t\t// System located in " + descriptor.LocatedHeaderFile + "\n";
	result.append(SystemRegistrationCallSource);

	std::regex const systemFullTypeKeywordPattern(R"(\$\{SYSTEM_FULL_TYPE\})");
	std::regex const systemNameKeywordPattern(R"(\$\{SYSTEM_NAME\})");
	std::regex const systemTypeKeywordPattern(R"(\$\{SYSTEM_TYPE\})");

	result = std::regex_replace(result, systemFullTypeKeywordPattern, systemFullType);
	result = std::regex_replace(result, systemNameKeywordPattern, systemName);
	result = std::regex_replace(result, systemTypeKeywordPattern, systemType);

	return result;
}