#include <iostream>
#include <fstream>
#include <filesystem>
#include <regex>
#include <string>

#include <cstdlib>

char const* Headers =
R"(
#include "UserTypeRegister.h"

#include "TypeRegistry.h"
#include "Util/Logger.h"
#include "ImGui/ImGuiUtil.h"

// Insert user headers here...
)";

char const* TypeRegistrationFunctionBodyStart =
R"(
namespace
{
	void userRegisterTypeFunction()
	{
		DYE_LOG("<< Register User Types to DYEditor::TypeRegistry >>");

		// Insert user type registration here...
)";

char const* TypeRegistrationFunctionBodyEnd =
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

std::string createComponentTypeRegistrationCallSource(std::string const& componentName, std::string const& componentType)
{
	char const *UserTypeRegistrationCallSource =
		R"(
		TypeRegistry::RegisterComponentType<${COMPONENT_TYPE}>
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

int main(int argc, char* argv[])
{
	std::cout << "Number Of Arguments: " << argc << std::endl;

	if (argc <= 3)
	{
		return 0;
	}

	for (int i = 1; i < argc; ++i)
	{
		printf("File: %s\n", argv[i]);
	}

	char const *input = argv[1];
	char const *output = argv[2];
	char const *testFile = argv[3];
	char const *sourceFiles = argv[4];

	//printf("Input File: %s\n", input);
	//printf("Output File: %s\n", output);
	//printf("Source File: %s\n", sourceFiles);

	// We delete the existing output file before copying,
	// because filesystem::copy_file with overwrite option is still buggy in some compilers :(
	if (std::filesystem::exists(output))
	{
		std::filesystem::remove(output);
	}

	std::error_code errorCode;
	bool const copyFileSuccess = std::filesystem::copy_file(input, output,
															std::filesystem::copy_options::overwrite_existing,
															errorCode);
	if (!copyFileSuccess)
	{
		std::cout << "Copy Input to Output Failed: " << errorCode.message() << std::endl;
	}

	if (std::filesystem::exists(testFile))
	{
		// Try parser
		std::ifstream file;
		file.open(testFile, std::ios::in);

		std::regex componentSyntaxPattern(
			"^\\s*DYE_COMPONENT\\(\"([a-zA-Z]+[a-zA-Z0-9_]*?)\",\\s*([a-zA-Z0-9_]+(::[a-zA-Z0-9_]+)*)\\)\\s*$");
		std::smatch match;

		std::string line;
		while (std::getline(file, line))
		{
			if (!std::regex_match(line, match, componentSyntaxPattern))
			{
				continue;
			}

			std::cout << "Component Name: " << match[1].str() << std::endl;
			std::cout << "Component Type: " << match[2].str() << std::endl;
			std::cout << "Function Call: " << createComponentTypeRegistrationCallSource(match[1], match[2]) << std::endl;
		}
	}

	return 0;
}