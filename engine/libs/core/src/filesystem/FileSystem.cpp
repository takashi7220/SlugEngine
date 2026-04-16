#pragma once

#include "core/filesystem/FileSystem.hpp"
#include "core/filesystem/DefaultFileSystemController.hpp"
#include "core/memory/UniquePtr.hpp"

namespace slug::core
{

static TUniquePtr<IFileSystemController> FILE_SYSTEM_CONTROLLER = nullptr;

FileBlobPtr FileSystem::Read(core::StringView filePath) 
{
    return GetController()->Read(filePath);
}

bool FileSystem::Read(core::StringView filePath, size_t& size, void* data) 
{
    return GetController()->Read(filePath, size, data);
}

bool FileSystem::Write(core::StringView filePath, const FileBlobPtr& data, bool makeDir)
{
    return GetController()->Write(filePath, data, makeDir);
}

bool FileSystem::Write(core::StringView filePath, size_t size, void* data, bool makeDir)
{
    return GetController()->Write(filePath, size, data, makeDir);
}

String FileSystem::GetExtension(core::StringView filePath)
{
    return GetController()->GetExtension(filePath);
}

String FileSystem::GetFilename(core::StringView filePath, bool hasExtension) 
{
    return GetController()->GetFilename(filePath, hasExtension);
}

String FileSystem::GetDirectory(core::StringView filePath) 
{
    return GetController()->GetDirectory(filePath);
}

bool FileSystem::MakeDirectory(core::StringView directoryPath) 
{
    return GetController()->MakeDirectory(directoryPath);
}

bool FileSystem::CheckFileExists(core::StringView filePath)
{
    return GetController()->CheckDirectoryExists(filePath);
}

bool FileSystem::CheckDirectoryExists(core::StringView directoryPath) 
{
    return GetController()->CheckDirectoryExists(directoryPath);
}

uint32_t FileSystem::EnumrateFiles(core::StringView rootPath, TVector<String>& enumrateFiles, uint32_t recursive)
{
    return GetController()->EnumrateFiles(rootPath, enumrateFiles, recursive);
}

uint32_t FileSystem::EnumrateFiles(const TVector<core::StringView>& rootPaths, TVector<String>& enumrateFiles, uint32_t recursive)
{
    return GetController()->EnumrateFiles(rootPaths, enumrateFiles, recursive);
}

uint32_t FileSystem::EnumrateFiles(core::StringView rootPath, core::StringView searchFileName, TVector<String>& enumrateFiles, uint32_t recursive) 
{
    return GetController()->EnumrateFiles(rootPath, searchFileName, enumrateFiles, recursive);
}

uint32_t FileSystem::EnumrateFiles(const TVector<core::StringView>& rootPaths, core::StringView searchFileName, TVector<String>& enumrateFiles, uint32_t recursive)
{
    return GetController()->EnumrateFiles(rootPaths, searchFileName, enumrateFiles, recursive);
}

uint32_t FileSystem::EnumrateFiles(const TVector<core::StringView>& rootPaths, const TVector<core::StringView>& searchFileNames, TVector<String>& enumrateFiles, uint32_t recursive)
{
    return GetController()->EnumrateFiles(rootPaths, searchFileNames, enumrateFiles, recursive);
}

uint32_t FileSystem::EnumrateFilesExtension(core::StringView rootPath, core::StringView searchExtension, TVector<String>& enumrateFiles, uint32_t recursive)
{
    return GetController()->EnumrateFilesExtension(rootPath, searchExtension, enumrateFiles, recursive);
}

uint32_t FileSystem::EnumrateFilesExtension(const TVector<core::StringView>& rootPaths, core::StringView searchExtension, TVector<String>& enumrateFiles, uint32_t recursive)
{
    return GetController()->EnumrateFilesExtension(rootPaths, searchExtension, enumrateFiles, recursive);
}

uint32_t FileSystem::EnumrateFilesExtension(const TVector<core::StringView>& rootPaths, const TVector<core::StringView>& searchExtensions, TVector<String>& enumrateFiles, uint32_t recursive)
{
    return GetController()->EnumrateFilesExtension(rootPaths, searchExtensions, enumrateFiles, recursive);
}

String FileSystem::GetRootPath(core::StringView searchPath)
{
    return GetController()->GetRootPath();
}

IFileSystemController* FileSystem::GetController()
{
    if (!FILE_SYSTEM_CONTROLLER)
    {
        FILE_SYSTEM_CONTROLLER.reset(new DefaultFileSystemController());
    }
    return FILE_SYSTEM_CONTROLLER.get();
}
}
