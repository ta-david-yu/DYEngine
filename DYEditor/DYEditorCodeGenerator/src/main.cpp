#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <sstream>
#include <regex>

#include <cstdlib>

char const* GeneratedSourceCodeRelativeFilePath = "generated/UserTypeRegister.generated.cpp";


char const* WarningAndHeaders =
R"(//------------------------------------------------------------------------------
// <auto-generated>
//     This code was generated by DYEditorCodeGenerator.
//     Changes to this file may cause incorrect behavior and will be lost if
//     the code is regenerated.
// </auto-generated>
//------------------------------------------------------------------------------
#include "UserTypeRegister.h"

#include "TypeRegistry.h"
#include "Util/Logger.h"
#include "ImGui/ImGuiUtil.h"

// Insert user headers here...
)";

char const* TypeRegisterBodyStart =
R"(
namespace DYE::DYEditor
{
	void userRegisterTypeFunction()
	{
		// Insert user type registration here...
)";

char const* TypeRegisterBodyEnd =
R"(
	}

	class UserTypeRegister_Generated
	{
	public:
		UserTypeRegister_Generated()
		{
			DYE::DYEditor::AssignRegisterUserTypeFunctionPointer(userRegisterTypeFunction);
		}
	};

	[[maybe_unused]]
	static UserTypeRegister_Generated userTypeRegister_GeneratedInstance = UserTypeRegister_Generated();
}
)";

struct ComponentDescriptor
{
	std::string ComponentName;
	std::string ComponentType;
};

struct ParseResult
{
	bool HasDYEditorKeyword = false;
	std::vector<ComponentDescriptor> ComponentDescriptors;
};

ParseResult parseHeaderFile(std::filesystem::path const& absoluteFilePath);
std::string createComponentTypeRegistrationCallSource(std::string const& componentName, std::string const& componentType);

int main(int argc, char* argv[])
{
	std::cout << "Running DYEditorCodeGenerator..." << std::endl;

	if (argc < 2)
	{
		std::cout << "There should be at least 1 parameter as the source directory "
					 "of the target header files for code generation." << std::endl;
		return 0;
	}

	char const* programName = argv[0];
	std::string const srcDirectory(argv[1]);

	if (!std::filesystem::exists(srcDirectory))
	{
		std::printf("The given source directory '%s' doesn't exist.\n", srcDirectory.c_str());
		//return 0;
	}

	int const numberOfHeaderFiles = argc - 2;
	std::printf("The following %d files will be parsed for generating corresponding code.\n", numberOfHeaderFiles);

	// Parse the files.
	std::vector<std::string> headerFilesToInclude; headerFilesToInclude.reserve(numberOfHeaderFiles);
	std::vector<ComponentDescriptor> componentDescriptors;

	for (int headerFileIndex = 2; headerFileIndex < argc; ++headerFileIndex)
	{
		std::filesystem::path absoluteFilePath = srcDirectory;
		absoluteFilePath.append(argv[headerFileIndex]);

		if (std::filesystem::exists(absoluteFilePath))
		{
			std::printf("\tFile: %ls\n", absoluteFilePath.c_str());
		}
		else
		{
			std::printf("\tNon-existing file: %ls\n", absoluteFilePath.c_str());
			continue;
		}

		ParseResult const result = parseHeaderFile(absoluteFilePath);
		if (!result.HasDYEditorKeyword)
		{
			continue;
		}

		headerFilesToInclude.emplace_back(argv[headerFileIndex]);
		componentDescriptors.insert(componentDescriptors.end(), result.ComponentDescriptors.begin(), result.ComponentDescriptors.end());
	}

	// Generate user type register code based on the parsed result.
	std::stringstream generatedSourceCodeStream;

	generatedSourceCodeStream << WarningAndHeaders;

	std::printf("Headers with DYEditor keywords: \n");
	for (auto const& headerFile : headerFilesToInclude)
	{
		std::printf("\t%s\n", headerFile.c_str());

		// Insert header includes.
		generatedSourceCodeStream << R"(#include ")" << headerFile << R"(")" << std::endl;
	}

	generatedSourceCodeStream << TypeRegisterBodyStart;

	std::printf("Registered components: \n");
	std::cout << "Registered components: " << std::endl;
	for (auto const& componentDescriptor : componentDescriptors)
	{
		std::printf("\tname = %s, type = %s\n", componentDescriptor.ComponentName.c_str(), componentDescriptor.ComponentType.c_str());

		// Insert component type registration calls.
		generatedSourceCodeStream << createComponentTypeRegistrationCallSource(componentDescriptor.ComponentName, componentDescriptor.ComponentType);
	}

	generatedSourceCodeStream << TypeRegisterBodyEnd;

	// Write the generated source code into target file.
	std::filesystem::path outputFilePath(srcDirectory);
	outputFilePath /= GeneratedSourceCodeRelativeFilePath;
	std::ofstream outputFileStream;

	std::printf("Generate code at '%ls'. \n", outputFilePath.c_str());

	outputFileStream.open(outputFilePath, std::ios::out | std::ios::trunc);
	outputFileStream << generatedSourceCodeStream.rdbuf();
	outputFileStream.close();

	return 0;
}

ParseResult parseHeaderFile(std::filesystem::path const& absoluteFilePath)
{
	if (!std::filesystem::exists(absoluteFilePath))
	{
		return ParseResult {.HasDYEditorKeyword = false};
	}

	ParseResult result;

	// Try parser
	std::ifstream file;
	file.open(absoluteFilePath, std::ios::in);

	std::regex const componentSyntaxPattern(
		"^\\s*DYE_COMPONENT\\(\"([a-zA-Z]+[a-zA-Z0-9_]*?)\",\\s*([a-zA-Z0-9_]+(::[a-zA-Z0-9_]+)*)\\)\\s*$");
	std::smatch match;

	std::string line;
	while (std::getline(file, line))
	{
		if (std::regex_match(line, match, componentSyntaxPattern))
		{
			result.HasDYEditorKeyword |= true;

			std::string const& componentName = match[1].str();
			std::string const& componentType = match[2].str();

			result.ComponentDescriptors.emplace_back(ComponentDescriptor { componentName, componentType });
			continue;
		}

		// TODO: parse DYE_PROPERTY & DYE_SYSTEM
	}

	return result;
}

std::string createComponentTypeRegistrationCallSource(std::string const& componentName, std::string const& componentType)
{
	char const *UserTypeRegistrationCallSource =
R"(		TypeRegistry::RegisterComponentType<${COMPONENT_TYPE}>
			(
				"${COMPONENT_NAME}",
				ComponentTypeFunctionCollection
					{
						.DrawInspector = [](Entity &entity)
						{
							ImGui::TextWrapped("${COMPONENT_TYPE}");
							return false;
						}
					}
			);

)";

	std::string result(UserTypeRegistrationCallSource);

	std::regex const componentNameKeywordPattern(R"(\$\{COMPONENT_NAME\})");
	std::regex const componentTypeKeywordPattern(R"(\$\{COMPONENT_TYPE\})");

	result = std::regex_replace(result, componentNameKeywordPattern, componentName);
	result = std::regex_replace(result, componentTypeKeywordPattern, componentType);

	return result;
}
