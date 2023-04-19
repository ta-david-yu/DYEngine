#include "FileSystem/FileSystem.h"
#include <filesystem>
#include <system_error>

namespace DYE
{
	namespace FileSystem
	{
		bool FileExists(std::filesystem::path const &path)
		{
			std::error_code existsErrorCode;
			bool const exists = std::filesystem::exists(path, existsErrorCode);
			if (!exists)
			{
				return false;
			}

			std::error_code isFileErrorCode;
			bool const isFile = std::filesystem::is_regular_file(path, isFileErrorCode);
			if (!isFile)
			{
				return false;
			}

			return true;
		}

	}
}