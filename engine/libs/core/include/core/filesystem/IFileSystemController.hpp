#pragma once

#include "core/filesystem/IFileHandle.hpp"
#include "core/filesystem/FileBlob.hpp"
#include "core/container/String.hpp"

namespace slug::core
{
struct FileOepnDesc
{
    core::StringView path;
    bool async = false;
    bool sequential = false;
    bool randomAccess = true;
};

class IFileSystemController 
{
public:
    virtual FileHandlePtr Open(const FileOepnDesc& desc) = 0;
    virtual FileBlobPtr Read(core::StringView filePath) = 0;
    virtual bool Read(core::StringView filePath, size_t& size, void* data) = 0;

    virtual String ReadText(core::StringView filePath) = 0;
    virtual bool ReadText(core::StringView filePath, String& out) = 0;

    virtual bool Write(core::StringView filePath, const FileBlobPtr& data, bool makeDir = true) = 0;
    virtual bool Write(core::StringView filePath, size_t size, void* data, bool makeDir = true) = 0;

    virtual bool WriteText(core::StringView filePath, const core::String& text, bool makeDir = true) = 0;

    virtual String GetExtension(core::StringView filePath) = 0;
    virtual String GetFilename(core::StringView filePath, bool hasExtension = true) = 0;
    virtual String GetDirectory(core::StringView filePath) = 0;

    virtual bool MakeDirectory(core::StringView directoryPath) = 0;

    virtual bool CheckFileExists(core::StringView filePath) = 0;
    virtual bool CheckDirectoryExists(core::StringView directoryPath) = 0;

    virtual uint32_t EnumrateFiles(core::StringView rootPath, TVector<String>& enumrateFiles, uint32_t recursive = 100) = 0;
    virtual uint32_t EnumrateFiles(const TVector<core::StringView>& rootPaths, TVector<String>& enumrateFiles, uint32_t recursive = 100) = 0;

    virtual uint32_t EnumrateFiles(core::StringView rootPath, core::StringView searchFileName, TVector<String>& enumrateFiles, uint32_t recursive = 100) = 0;
    virtual uint32_t EnumrateFiles(const TVector<core::StringView>& rootPaths, core::StringView searchFileName, TVector<String>& enumrateFiles, uint32_t recursive = 100) = 0;
    virtual uint32_t EnumrateFiles(const TVector<core::StringView>& rootPaths, const TVector<core::StringView>& searchFileNames, TVector<String>& enumrateFiles, uint32_t recursive = 100) = 0;

    virtual uint32_t EnumrateFilesExtension(core::StringView rootPath, core::StringView searchExtension, TVector<String>& enumrateFiles, uint32_t recursive = 100) = 0;
    virtual uint32_t EnumrateFilesExtension(const TVector<core::StringView>& rootPaths, core::StringView searchExtension, TVector<String>& enumrateFiles, uint32_t recursive = 100) = 0;
    virtual uint32_t EnumrateFilesExtension(const TVector<core::StringView>& rootPaths, const TVector<core::StringView>& searchExtensions, TVector<String>& enumrateFiles, uint32_t recursive = 100) = 0;

    virtual String GetRootPath(core::StringView searchPath = ".root") = 0;
};
}
