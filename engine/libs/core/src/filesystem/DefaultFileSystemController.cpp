#pragma once

#include "core/filesystem/DefaultFileSystemController.hpp"
#include "core/io/Path.hpp"
#include <fstream>

#if defined(PLATFORM_WINDOWS)
#include <Windows.h>
#endif

namespace slug::core
{
#if defined(PLATFORM_WINDOWS)
slug::core::String GetCurrentDirctory()
{
    char cdir[255];
    GetCurrentDirectory(255, cdir);
    return String(cdir);
}
#else
slug::core::String GetCurrentDirctory()
{
    return String("");
}
#endif
FileBlobPtr DefaultFileSystemController::Read(core::StringView filePath)
{
    std::ifstream inFile(filePath.data(), std::ios::binary);
    TVector<char> data = {};
    if (inFile.is_open()) 
    {
        inFile.seekg(0, std::ios::end);
        std::streamsize size = inFile.tellg();
        inFile.seekg(0, std::ios::beg);

        data.resize(size);
        inFile.read(data.data(), size);
        inFile.close();
    }
    FileBlobPtr ret = nullptr;
    ret.reset(new FileBlob((void*)data.data(), data.size()));
    return ret;
}

bool DefaultFileSystemController::Read(core::StringView filePath, size_t& size, void* data)
{
    std::ifstream inFile(filePath.data(), std::ios::binary);
    TVector<char> tmpData = {};
    if (inFile.is_open())
    {
        inFile.seekg(0, std::ios::end);
        std::streamsize fileSize = inFile.tellg();
        inFile.seekg(0, std::ios::beg);

        tmpData.resize(fileSize);
        inFile.read(tmpData.data(), fileSize);
        inFile.close();

        data = (void*)tmpData.data();
        fileSize = tmpData.size();
        return true;
    }
    return false;
}

bool DefaultFileSystemController::Write(core::StringView filePath, const FileBlobPtr& data, bool makeDir)
{
    if (makeDir) 
    {
        String dirName = this->GetDirectory(filePath);
        this->MakeDirectory(dirName.c_str());
    }
    std::ofstream outFile(filePath.data(), std::ios::binary);
    if (outFile.is_open()) 
    {
        outFile.write((const char*)data.get()->data(), data.get()->size());
        outFile.close();
        return true;
    }
    return false;
}

bool DefaultFileSystemController::Write(core::StringView filePath, size_t size, void* data, bool makeDir)
{
    if (makeDir)
    {
        String dirName = this->GetDirectory(filePath);
        this->MakeDirectory(dirName.c_str());
    }

    std::ofstream outFile(filePath.data(), std::ios::binary);
    if (outFile.is_open())
    {
        outFile.write((const char*)data, size);
        outFile.close();
        return true;
    }
    return false;
}


String DefaultFileSystemController::GetExtension(core::StringView filePath)
{
    return String(Path(filePath).extension().string().c_str());
}

String DefaultFileSystemController::GetFilename(core::StringView filePath, bool hasExtension)
{
    Path tmpPath = Path(filePath).filename();
    if (hasExtension)
    {
        return String(tmpPath.string().c_str());
    }
    else 
    {
        return String(tmpPath.replace_extension("").string().c_str());
    }
}

String DefaultFileSystemController::GetDirectory(core::StringView filePath)
{
    Path tmpPath(filePath);
    return String(tmpPath.parent_path().string().c_str());
}

bool DefaultFileSystemController::MakeDirectory(core::StringView directoryPath)
{
    return PathUtility::CreateDirectories(Path(directoryPath));
}

bool DefaultFileSystemController::CheckFileExists(core::StringView filePath)
{
    return PathUtility::CheckFileExists(Path(filePath));
}

bool DefaultFileSystemController::CheckDirectoryExists(core::StringView directoryPath)
{
    return PathUtility::CheckDirectory(Path(directoryPath));
}

uint32_t DefaultFileSystemController::EnumrateFiles(core::StringView rootPath, TVector<String>& enumrateFiles, uint32_t recursive)
{
    Path root(rootPath);
    if (recursive > 0)
    {
        for (auto& p : RecursiveDirectoryIterator(root))
        {
            enumrateFiles.push_back(p.path().string().c_str());
        }
    } else
    {
        for (auto& p : DirectoryIterator(root))
        {
            enumrateFiles.push_back(p.path().string().c_str());
        }
    }
    return static_cast<uint32_t>(enumrateFiles.size());
}

uint32_t DefaultFileSystemController::EnumrateFiles(const TVector<core::StringView>& rootPaths, TVector<String>& enumrateFiles, uint32_t recursive)
{
    for (auto& rootPath : rootPaths)
    {
        EnumrateFiles(rootPath, enumrateFiles, recursive);
    }
    return static_cast<uint32_t>(enumrateFiles.size());
}

uint32_t DefaultFileSystemController::EnumrateFiles(core::StringView rootPath, core::StringView searchFileName, TVector<String>& enumrateFiles, uint32_t recursive)
{
    Path root(rootPath);
    if (recursive > 0)
    {
        for (auto& p : RecursiveDirectoryIterator(root))
        {
            if (p.path().filename() == searchFileName)
            {
                enumrateFiles.push_back(p.path().string().c_str());
            }
        }
    }
    else
    {
        for (auto& p : DirectoryIterator(root))
        {
            if (p.path().filename() == searchFileName)
            {
                enumrateFiles.push_back(p.path().string().c_str());
            }
        }
    }
    return static_cast<uint32_t>(enumrateFiles.size());
}

uint32_t DefaultFileSystemController::EnumrateFiles(const TVector<core::StringView>& rootPaths, core::StringView searchFileName, TVector<String>& enumrateFiles, uint32_t recursive)
{
    for(auto& rootPath : rootPaths)
    {
        EnumrateFiles(rootPath, searchFileName, enumrateFiles, recursive);
    }
    return static_cast<uint32_t>(enumrateFiles.size());
}

uint32_t DefaultFileSystemController::EnumrateFiles(const TVector<core::StringView>& rootPaths, const TVector<core::StringView>& searchFileNames, TVector<String>& enumrateFiles, uint32_t recursive)
{
    for (auto& fileName : searchFileNames)
    {
        EnumrateFiles(rootPaths, fileName, enumrateFiles, recursive);
    }
    return static_cast<uint32_t>(enumrateFiles.size());
}

uint32_t DefaultFileSystemController::EnumrateFilesExtension(core::StringView rootPath, core::StringView searchExtension, TVector<String>& enumrateFiles, uint32_t recursive)
{
    Path root(rootPath);
    if (recursive > 0)
    {
        for (auto& p : RecursiveDirectoryIterator(root))
        {
            if (p.path().extension() == searchExtension)
            {
                enumrateFiles.push_back(p.path().string().c_str());
            }
        }
    }
    else
    {
        for (auto& p : DirectoryIterator(root))
        {
            if (p.path().extension() == searchExtension)
            {
                enumrateFiles.push_back(p.path().string().c_str());
            }
        }
    }
    return static_cast<uint32_t>(enumrateFiles.size());

}

uint32_t DefaultFileSystemController::EnumrateFilesExtension(const TVector<core::StringView>& rootPaths, core::StringView searchExtension, TVector<String>& enumrateFiles, uint32_t recursive)
{
    for (auto& rootPath : rootPaths)
    {
        EnumrateFilesExtension(rootPath, searchExtension, enumrateFiles, recursive);
    }
    return static_cast<uint32_t>(enumrateFiles.size());
}

uint32_t DefaultFileSystemController::EnumrateFilesExtension(const TVector<core::StringView>& rootPaths, const TVector<core::StringView>& searchExtensions, TVector<String>& enumrateFiles, uint32_t recursive)
{
    for (auto& extension : searchExtensions)
    {
        EnumrateFilesExtension(rootPaths, extension, enumrateFiles, recursive);
    }
    return static_cast<uint32_t>(enumrateFiles.size());
}

String DefaultFileSystemController::GetRootPath(core::StringView rootPath)
{
    Path currentPath(GetCurrentDirctory());
    while (currentPath.has_parent_path()) 
    {
        Path filePath = currentPath / rootPath;
        if (CheckFileExists(filePath.string().c_str()) || 
            CheckDirectoryExists(filePath.string().c_str())) 
        {
            return String(currentPath.string().c_str());
        }
        currentPath = currentPath.parent_path();
    }
    return GetCurrentDirctory();
}
}
