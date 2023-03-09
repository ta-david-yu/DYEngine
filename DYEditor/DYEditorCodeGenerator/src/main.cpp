#include <iostream>
#include <fstream>
#include <filesystem>

#include <cstdlib>

int main(int argc, char* argv[])
{
	std::cout << "Number Of Arguments: " << argc << std::endl;

	if (argc > 2)
	{
		char const* input = argv[1];
		char const* output = argv[2];

		printf("Input File: %s\n", input);
		printf("Output File: %s\n", output);

		// We delete the existing output file before copying,
		// because filesystem::copy_file with overwrite option is still buggy in some compilers :(
		if (std::filesystem::exists(output))
		{
			std::filesystem::remove(output);
		}

		std::error_code errorCode;
		bool const copyFileSuccess = std::filesystem::copy_file(input, output, std::filesystem::copy_options::overwrite_existing, errorCode);

		if (!copyFileSuccess)
		{
			std::cout << "Copy Input to Output Failed: " << errorCode.message() << std::endl;
		}
	}

	return 0;
}