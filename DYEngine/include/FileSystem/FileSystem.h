#pragma once

#include <filesystem>

namespace DYE
{
    namespace FileSystem
    {
        bool FileExists(std::filesystem::path const &path);
    };
}
