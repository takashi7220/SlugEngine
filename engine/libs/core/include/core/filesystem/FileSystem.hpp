#pragma once

#include "core/filesystem/IFileSystemController.hpp"
#include "core/pattern/Singleton.hpp"
#include <fstream>

namespace slug::core
{

using Ifstream = std::ifstream;
using Ofstream = std::ofstream;

template<typename T>
concept DerivedFromFileSystemController = std::is_base_of_v<IFileSystemController, T>;

class FileSystem
{
public:
    static FileBlobPtr Read(core::StringView filePath);
    static bool Read(core::StringView filePath, size_t& size, void* data);

    static bool Write(core::StringView filePath, const FileBlobPtr& data, bool makeDir = true);
    static bool Write(core::StringView filePath, size_t size, void* data, bool makeDir = true);

    static String GetExtension(core::StringView filePath);
    static String GetFilename(core::StringView filePath, bool hasExtension = true);
    static String GetDirectory(core::StringView filePath);

    static bool MakeDirectory(core::StringView directoryPath);
    static bool CheckFileExists(core::StringView filePath);
    static bool CheckDirectoryExists(core::StringView directoryPath);

    static uint32_t EnumrateFiles(core::StringView rootPath, TVector<String>& enumrateFiles, uint32_t recursive = 100);
    static uint32_t EnumrateFiles(const TVector<core::StringView>& rootPaths, TVector<String>& enumrateFiles, uint32_t recursive = 100);

    static uint32_t EnumrateFiles(core::StringView rootPath, core::StringView searchFileName, TVector<String>& enumrateFiles, uint32_t recursive = 100);
    static uint32_t EnumrateFiles(const TVector<core::StringView>& rootPaths, core::StringView searchFileName, TVector<String>& enumrateFiles, uint32_t recursive = 100);
    static uint32_t EnumrateFiles(const TVector<core::StringView>& rootPaths, const TVector<core::StringView>& searchFileNames, TVector<String>& enumrateFiles, uint32_t recursive = 100);

    static uint32_t EnumrateFilesExtension(core::StringView rootPath, core::StringView searchExtension, TVector<String>& enumrateFiles, uint32_t recursive = 100);
    static uint32_t EnumrateFilesExtension(const TVector<core::StringView>& rootPaths, core::StringView searchExtension, TVector<String>& enumrateFiles, uint32_t recursive = 100);
    static uint32_t EnumrateFilesExtension(const TVector<core::StringView>& rootPaths, const TVector<core::StringView>& searchExtensions, TVector<String>& enumrateFiles, uint32_t recursive = 100);

    static String GetRootPath(core::StringView searchPath = ".root");
private:
    static IFileSystemController* GetController();
};
}
