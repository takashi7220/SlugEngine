#pragma once

#if defined(SLUG_USE_STL)
#include <filesystem>
#endif

namespace slug::core
{
#if defined(SLUG_USE_STL)
using Path = std::filesystem::path;   

class PathUtility
{
public:
    static bool CreateDirectory(const Path& path)
    {
        return std::filesystem::create_directory(path);
    }

    static bool CreateDirectories(const Path& path)
    {
        return std::filesystem::create_directories(path);
    }

    static bool CheckFileExists(const Path& path) 
    {
        return std::filesystem::exists(path);
    }

    static bool CheckDirectory(const Path& path) 
    {
        return std::filesystem::is_directory(path);
    }
};

using DirectoryIterator = std::filesystem::directory_iterator;
using RecursiveDirectoryIterator = std::filesystem::recursive_directory_iterator;
#endif

}
