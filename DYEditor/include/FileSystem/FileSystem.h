#pragma once

#include <filesystem>

namespace DYE::DYEditor
{
	namespace FileSystem
	{
		bool FileExists(std::filesystem::path const& path);
	};
}
