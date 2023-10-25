#include "PropertyCodeGeneration.h"
#include "ComponentCodeGeneration.h"
#include "SystemCodeGeneration.h"

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
#include "Type/UserTypeRegister.h"

#include "Util/Macro.h"
#include "Type/TypeRegistry.h"
#include "Serialization/SerializedObjectFactory.h"
#include "ImGui/ImGuiUtil.h"
#include "ImGui/EditorImGuiUtil.h"
#include "Undo/Undo.h"

// Insert user headers here...
)";

char const* TypeRegisterBodyStart =
R"(

namespace DYE::DYEditor
{
	/// A helper function that helps updating context data after a DrawControl call.
	/// Used by DrawInspector functions mostly.
	inline void updateContextAfterDrawControlCall(DrawComponentInspectorContext &context)
	{
		context.IsModificationActivated |= ImGuiUtil::IsControlActivated();
		context.IsModificationDeactivated |= ImGuiUtil::IsControlDeactivated();
		context.IsModificationDeactivatedAfterEdit |= ImGuiUtil::IsControlDeactivatedAfterEdit();
	}

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

struct ParseResult
{
	bool HasDYEditorKeyword = false;
	std::vector<ComponentDescriptor> ComponentDescriptors;
	std::vector<SystemDescriptor> SystemDescriptors;
};

ParseResult parseHeaderFile(std::filesystem::path const& sourceDirectory, std::filesystem::path const& relativeFilePath);

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
	std::vector<SystemDescriptor> systemDescriptors;

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
		systemDescriptors.insert(systemDescriptors.end(), result.SystemDescriptors.begin(), result.SystemDescriptors.end());
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
		if (componentDescriptor.HasOptionalDisplayName)
		{
			std::printf("\ttypeName = %s, numberOfProperties = %zu, optionalDisplayName = %s, formerlyKnowNamesCount = %zu, hintCount = %zu\n",
						componentDescriptor.FullType.c_str(),
						componentDescriptor.Properties.size(),
						componentDescriptor.OptionalDisplayName.c_str(),
						componentDescriptor.FormerlyKnownNames.size(),
						componentDescriptor.UseWithComponentTypeHints.size());
		}
		else
		{
			std::printf("\ttypename = %s, numberOfProperties = %zu, formerlyKnowNamesCount = %zu, hintCount = %zu\n",
						componentDescriptor.FullType.c_str(),
						componentDescriptor.Properties.size(),
						componentDescriptor.FormerlyKnownNames.size(),
						componentDescriptor.UseWithComponentTypeHints.size());
		}

		// Insert component type registration calls.
		generatedSourceCodeStream << ComponentDescriptorToTypeRegistrationCallSource(componentDescriptor);
	}

	std::printf("Registered systems: \n");
	for (auto const& systemDescriptor : systemDescriptors)
	{
		std::printf("\ttypeName = %s, name = %s, formerlyKnowNamesCount = %zu\n",
					systemDescriptor.FullType.c_str(),
					systemDescriptor.CustomName.c_str(),
					systemDescriptor.FormerlyKnownNames.size());

		// Insert component type registration calls.
		generatedSourceCodeStream << SystemDescriptorToTypeRegistrationCallSource(systemDescriptor);
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
	std::regex const typeWithNamespacesPattern(R"(^((?:\w+::)*)(\w+)$)");
	std::regex const dyePropertyKeywordPattern(
		R"(^\s*DYE_PROPERTY\(\)\s*$)"
	);
	std::regex const variableDeclarationPattern(
		R"((?:(?:[a-zA-Z_][a-zA-Z0-9_]*)+::)*([a-zA-Z_][a-zA-Z0-9_]*)\s*(?:const\s+)?([a-zA-Z_][a-zA-Z0-9_]*)\s*(?:=\s*([^;]*))?;)"
	);
	std::regex const dyeFormerlyKnownAsKeywordPattern(
		R"lit(\s*DYE_FORMERLY_KNOWN_AS\(\s*"([^"]+?)"\s*\)\s*$)lit"
	);

#if defined(_MSC_VER)
	std::regex const dyeComponentKeywordPattern(
		R"lit(^\s*DYE_COMPONENT\(\s*([[:alnum:]_]+(?:\s*::\s*[[:alnum:]_]+)*)(?:\s*,\s*"([[:alpha:]][\w\s]*?)")?\s*\)\s*$)lit"
	);
	std::regex const dyeSystemKeywordPattern(
		R"lit(^\s*DYE_SYSTEM\(\s*([[:alnum:]_]+(?:\s*::\s*[[:alnum:]_]+)*)(?:\s*,\s*"([[:alpha:]][\w\s]*?)")?\s*\)\s*$)lit"
	);
	std::regex  const dyeUseWithComponentKeywordPattern(
		R"lit(\s*DYE_USE_WITH_COMPONENT_HINT\(\s*([[:alnum:]_]+(?:\s*::\s*[[:alnum:]_]+)*)\)\s*$)lit"
	);
#else
	std::regex const dyeComponentKeywordPattern(
		R"lit(^\s*DYE_COMPONENT\(\s*([a-zA-Z0-9_]+(?:\s*::\s*[a-zA-Z0-9_]+)*)(?:\s*,\s*"([a-zA-Z][\w\s]*?)")?\s*\)\s*$)lit"
	);
	std::regex const dyeSystemKeywordPattern(
		R"lit(^\s*DYE_SYSTEM\(\s*([a-zA-Z0-9_]+(?:\s*::\s*[a-zA-Z0-9_]+)*)(?:\s*,\s*"([a-zA-Z][\w\s]*?)")?\s*\)\s*$)lit"
	);
	std::regex const dyeUseWithComponentKeywordPattern(
		R"lit(\s*DYE_USE_WITH_COMPONENT_HINT\(\s*([a-zA-Z0-9_]+(?:\s*::\s*[a-zA-Z0-9_]+)*)\)\s*$)lit"
	);
#endif

	std::smatch match;

	std::string line;

	bool isInComponentScope = false;
	bool nextLineShouldBeVariableDeclaration = false;

	bool isInSystemScope = false;

	ComponentDescriptor currentComponentScopeDescriptor;
	SystemDescriptor currentSystemScopeDescriptor;

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
				std::string const& typeSpecifierString = match[1].str();
				PropertyType const& typeSpecifier = CastTypeSpecifierStringToPropertyType(typeSpecifierString);
				std::string const& variableName = match[2].str();
				bool const hasDefaultValue = !match[3].str().empty();
				std::string const& defaultValueString = hasDefaultValue? match[3].str() : "";
				bool const isConst = std::regex_search(line, match, constKeywordPattern);

				currentComponentScopeDescriptor.Properties.emplace_back
					(
						PropertyDescriptor
							{
								.TypeSpecifierString = typeSpecifierString,
								.TypeSpecifier = typeSpecifier,
								.VariableName = variableName,
								.HasDefaultValue = hasDefaultValue,
								.DefaultValueString = defaultValueString,
								.IsConstant = isConst,
							}
					);

				if (!isConst)
				{
					if (hasDefaultValue)
					{
						std::printf("\t\tDYE_PROPERTY '%s::%s' of type '%s' is registered. (Default value = %s)\n",
									currentComponentScopeDescriptor.FullType.c_str(), variableName.c_str(),
									typeSpecifierString.c_str(), defaultValueString.c_str());
					}
					else
					{
						std::printf("\t\tDYE_PROPERTY '%s::%s' of type '%s' is registered.\n",
									currentComponentScopeDescriptor.FullType.c_str(), variableName.c_str(),
									typeSpecifierString.c_str());
					}
				}
				else
				{
					std::printf("\t\tDYE_PROPERTY at line %d is ignored because the followed variable declaration ('%s::%s' of type '%s') is a constant.\n",
								lineCount - 1,
								currentComponentScopeDescriptor.FullType.c_str(), variableName.c_str(), typeSpecifierString.c_str());
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
			// Some Examples:
			// DYE_COMPONENT(DYE::DYEditor::LocalTransformComponent, "Local Transform")
			// DYE_COMPONENT(HasAngularVelocity)

			result.HasDYEditorKeyword |= true;

			if (isInComponentScope)
			{
				// We were in another DYE_COMPONENT body, flush the descriptor into the result.
				result.ComponentDescriptors.emplace_back(currentComponentScopeDescriptor);
				isInComponentScope = false;
			}

			if (isInSystemScope)
			{
				// We were in another DYE_SYSTEM body, flush the descriptor into the result.
				result.SystemDescriptors.emplace_back(currentSystemScopeDescriptor);
				isInSystemScope = false;
			}

			std::string const& componentFullTypeName = match[1].str();
			std::string const& componentDisplayName = match[2].str();

			bool hasOptionalDisplayName = !componentDisplayName.empty();
			if (hasOptionalDisplayName)
			{
				currentComponentScopeDescriptor = ComponentDescriptor{
					.LocatedHeaderFile = relativeFilePath.string(),
					.FullType = componentFullTypeName,
					.HasOptionalDisplayName = true,
					.OptionalDisplayName = componentDisplayName,
				};
			}
			else
			{
				currentComponentScopeDescriptor = ComponentDescriptor{
					.LocatedHeaderFile = relativeFilePath.string(),
					.FullType = componentFullTypeName,
					.HasOptionalDisplayName = false
				};
			}

			isInComponentScope = true;

			continue;
		}

		// DYE_SYSTEM
		bool const isDYESystemKeyword = std::regex_match(line, match, dyeSystemKeywordPattern);
		if (isDYESystemKeyword)
		{
			result.HasDYEditorKeyword |= true;

			if (isInComponentScope)
			{
				// We were in another DYE_COMPONENT body, flush the descriptor into the result.
				result.ComponentDescriptors.emplace_back(currentComponentScopeDescriptor);
				isInComponentScope = false;
			}

			if (isInSystemScope)
			{
				// We were in another DYE_SYSTEM body, flush the descriptor into the result.
				result.SystemDescriptors.emplace_back(currentSystemScopeDescriptor);
				isInSystemScope = false;
			}

			std::string const& systemType = match[1].str();
			std::string const& systemName = match[2].str();

			currentSystemScopeDescriptor = SystemDescriptor{
				.LocatedHeaderFile = relativeFilePath.string(),
				.FullType = systemType,
				.CustomName = systemName,
			};

			isInSystemScope = true;

			continue;
		}

		if (isInComponentScope)
		{
			// DYE_PROPERTY
			// We are inside a DYEComponent body, search for DYE_PROPERTY keyword.
			bool const isDYEPropertyKeyword = std::regex_match(line, match, dyePropertyKeywordPattern);
			if (isDYEPropertyKeyword)
			{
				nextLineShouldBeVariableDeclaration = true;
				continue;
			}

			// DYE_FORMERLY_KNOWN_AS
			bool const isDYEFormerlyKnownAsKeyword = std::regex_match(line, match, dyeFormerlyKnownAsKeywordPattern);
			if (isDYEFormerlyKnownAsKeyword)
			{
				std::string const& formerlyKnownTypeName = match[1];
				currentComponentScopeDescriptor.FormerlyKnownNames.push_back(formerlyKnownTypeName);
				continue;
			}

			// DYE_USE_WITH_COMPONENT_HINT
			bool const isDYEUseWithComponentHintKeyword = std::regex_match(line, match, dyeUseWithComponentKeywordPattern);
			if (isDYEUseWithComponentHintKeyword)
			{
				std::string const& useWithComponentTypeName = match[1];
				currentComponentScopeDescriptor.UseWithComponentTypeHints.push_back(useWithComponentTypeName);
				continue;
			}
		}

		if (isInSystemScope)
		{
			// DYE_FORMERLY_KNOWN_AS
			bool const isDYEFormerlyKnownAsKeyword = std::regex_match(line, match, dyeFormerlyKnownAsKeywordPattern);
			if (isDYEFormerlyKnownAsKeyword)
			{
				std::string const& formerlyKnownTypeName = match[1];
				currentSystemScopeDescriptor.FormerlyKnownNames.push_back(formerlyKnownTypeName);
				continue;
			}
		}
	}

	// In the end we want to enclose DYEComponent/System that hasn't been flushed to the result.
	if (isInComponentScope)
	{
		// We were in another DYEComponent body, flush the descriptor into the result.
		result.ComponentDescriptors.emplace_back(currentComponentScopeDescriptor);
		isInComponentScope = false;
	}

	if (isInSystemScope)
	{
		// We were in another DYE_SYSTEM body, flush the descriptor into the result.
		result.SystemDescriptors.emplace_back(currentSystemScopeDescriptor);
		isInSystemScope = false;
	}

	return result;
}
