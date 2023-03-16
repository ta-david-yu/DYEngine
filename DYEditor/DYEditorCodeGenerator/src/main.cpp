#include "PropertyCodeGeneration.h"

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
R"(	}

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
	std::string LocatedHeaderFile;
	std::string CustomName;
	std::string Type;
	std::vector<PropertyDescriptor> Properties;
};

struct ParseResult
{
	bool HasDYEditorKeyword = false;
	std::vector<ComponentDescriptor> ComponentDescriptors;
};

ParseResult parseHeaderFile(std::filesystem::path const& sourceDirectory, std::filesystem::path const& relativeFilePath);
std::string createComponentTypeRegistrationCallSource(ComponentDescriptor const& descriptor);

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

	// Parse files.
	std::vector<std::string> headerFilesToInclude; headerFilesToInclude.reserve(numberOfHeaderFiles);
	std::vector<ComponentDescriptor> componentDescriptors;

	for (int headerFileIndex = 2; headerFileIndex < argc; ++headerFileIndex)
	{
		std::filesystem::path const absoluteFilePath = srcDirectory / std::filesystem::path(argv[headerFileIndex]);

		if (std::filesystem::exists(absoluteFilePath))
		{
			std::printf("\tFile: %ls\n", absoluteFilePath.c_str());
		}
		else
		{
			std::printf("\tNon-existing file: %ls\n", absoluteFilePath.c_str());
			continue;
		}

		ParseResult const result = parseHeaderFile(srcDirectory, argv[headerFileIndex]);
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
	for (auto const& componentDescriptor : componentDescriptors)
	{
		std::printf("\tname = %s, type = %s, numberOfProperties = %zu\n",
					componentDescriptor.CustomName.c_str(),
					componentDescriptor.Type.c_str(),
					componentDescriptor.Properties.size());

		// Insert component type registration calls.
		generatedSourceCodeStream << createComponentTypeRegistrationCallSource(componentDescriptor);
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

ParseResult parseHeaderFile(std::filesystem::path const& sourceDirectory, std::filesystem::path const& relativeFilePath)
{
	std::filesystem::path const absoluteFilePath = sourceDirectory / relativeFilePath;
	if (!std::filesystem::exists(absoluteFilePath))
	{
		return ParseResult {.HasDYEditorKeyword = false};
	}

	ParseResult result;

	// Try parser
	std::ifstream file;
	file.open(absoluteFilePath, std::ios::in);

	std::regex const constKeywordPattern(R"(\bconst\b)");
	std::regex const dyeComponentKeywordPattern(
		"^\\s*DYE_COMPONENT\\(\"([a-zA-Z]+[a-zA-Z0-9_]*?)\",\\s*([a-zA-Z0-9_]+(::[a-zA-Z0-9_]+)*)\\)\\s*$"
	);
	std::regex const dyePropertyKeywordPattern(
		R"(^\s*DYE_PROPERTY\(\)\s*$)"
	);
	std::regex const variableDeclarationPattern(
		R"((?:(?:[a-zA-Z_][a-zA-Z0-9_]*)+::)*([a-zA-Z_][a-zA-Z0-9_]*)\s*(?:const\s+)?([a-zA-Z_][a-zA-Z0-9_]*)\s*(?:=\s*[^;]*)?;)"
	);
	std::smatch match;

	std::string line;
	bool isInComponentScope = false;
	bool isInSystemScope = false;
	bool nextLineShouldBeVariableDeclaration = false;

	ComponentDescriptor currentComponentScopeDescriptor;

	// Parse the file line by line.
	int lineCount = 0;
	while (std::getline(file, line))
	{
		lineCount++;

		if (nextLineShouldBeVariableDeclaration)
		{
			// The line after a DYE_PROPERTY should be a variable declaration.
			nextLineShouldBeVariableDeclaration = false;
			bool const isVariableDeclaration = std::regex_search(line, match, variableDeclarationPattern);
			if (isVariableDeclaration)
			{
				std::string const& typeSpecifier = match[1].str();
				std::string const& variableName = match[2].str();
				bool const isConst = std::regex_search(line, match, constKeywordPattern);

				currentComponentScopeDescriptor.Properties.emplace_back
					(
						PropertyDescriptor
							{
								.TypeSpecifier = typeSpecifier,
								.VariableName = variableName,
								.IsConstant = isConst
							}
					);

				if (!isConst)
				{
					std::printf("\t\tDYE_PROPERTY '%s::%s' of type '%s' is registered.\n",
								currentComponentScopeDescriptor.Type.c_str(), variableName.c_str(),
								typeSpecifier.c_str());
				}
				else
				{
					std::printf("\t\tDYE_PROPERTY at line %d is ignored because the followed variable declaration ('%s::%s' of type '%s') is a constant.\n",
								lineCount - 1,
								currentComponentScopeDescriptor.Type.c_str(), variableName.c_str(), typeSpecifier.c_str());
				}

				continue;
			}
			else
			{
				std::printf("\t\tDYE_PROPERTY at line %d is ignored because it is not followed by a valid variable declaration.\n", lineCount - 1);
			}
		}

		// DYE_COMPONENT
		bool const isDYEComponentKeyword = std::regex_match(line, match, dyeComponentKeywordPattern);
		if (isDYEComponentKeyword)
		{
			result.HasDYEditorKeyword |= true;

			if (isInComponentScope)
			{
				// We were in another DYE_COMPONENT body, flush the descriptor into the result.
				result.ComponentDescriptors.emplace_back(currentComponentScopeDescriptor);
				isInComponentScope = false;
			}
			else if (isInSystemScope)
			{
				// TODO: flush DYE_SYSTEM into the result.
				isInSystemScope = false;
			}

			std::string const& componentName = match[1].str();
			std::string const& componentType = match[2].str();

			currentComponentScopeDescriptor = ComponentDescriptor{
				.LocatedHeaderFile = relativeFilePath.string(),
				.CustomName = componentName,
				.Type = componentType
			};

			isInComponentScope = true;

			continue;
		}

		// TODO: parse DYE_SYSTEM

		if (isInComponentScope || isInSystemScope)
		{
			// We are inside a DYEComponent/System body, search for DYE_PROPERTY keyword.
			bool const isDYEPropertyKeyword = std::regex_match(line, match, dyePropertyKeywordPattern);
			if (!isDYEPropertyKeyword)
			{
				continue;
			}
			nextLineShouldBeVariableDeclaration = true;
		}
	}

	// In the end we want to enclose DYEComponent/System that hasn't been flushed to the result.
	if (isInComponentScope)
	{
		// We were in another DYEComponent body, flush the descriptor into the result.
		result.ComponentDescriptors.emplace_back(currentComponentScopeDescriptor);
		isInComponentScope = false;
	}
	else if (isInSystemScope)
	{
		// TODO: flush DYE_SYSTEM into the result.
		isInSystemScope = false;
	}

	return result;
}

char const *UserTypeRegistrationCallSourceStart =
R"(		TypeRegistry::RegisterComponentType<${COMPONENT_TYPE}>
			(
				"${COMPONENT_NAME}",
				ComponentTypeFunctionCollection
					{
						.DrawInspector = [](Entity &entity)
						{
							bool changed = false;
							ImGui::TextWrapped("${COMPONENT_TYPE}");
)";

char const* UserTypeRegistrationCallSourceEnd =
R"(							return changed;
						}
					}
			);

)";

std::string createComponentTypeRegistrationCallSource(ComponentDescriptor const& descriptor)
{
	auto const& componentName = descriptor.CustomName;
	auto const& componentType = descriptor.Type;

	std::string result = "\t\t// Component located in " + descriptor.LocatedHeaderFile + "\n";
	result.append(UserTypeRegistrationCallSourceStart);

	for (auto const& propertyDescriptor : descriptor.Properties)
	{
		result.append(PropertyDescriptorToImGuiUtilControlCallSource(descriptor.Type, propertyDescriptor));
	}

	result.append(UserTypeRegistrationCallSourceEnd);

	std::regex const componentNameKeywordPattern(R"(\$\{COMPONENT_NAME\})");
	std::regex const componentTypeKeywordPattern(R"(\$\{COMPONENT_TYPE\})");

	result = std::regex_replace(result, componentNameKeywordPattern, componentName);
	result = std::regex_replace(result, componentTypeKeywordPattern, componentType);

	return result;
}
