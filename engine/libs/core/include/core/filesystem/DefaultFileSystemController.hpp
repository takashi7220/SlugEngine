#pragma once

#include "core/filesystem/IFileSystemController.hpp"

namespace slug::core
{

class DefaultFileSystemController : public IFileSystemController
{
public:
    virtual FileBlobPtr Read(core::StringView filePath) override;
    virtual bool Read(core::StringView filePath, size_t& size, void* data) override;

    virtual bool Write(core::StringView filePath, const FileBlobPtr& data, bool makeDir = true) override;
    virtual bool Write(core::StringView filePath, size_t size, void* data, bool makeDir = true) override;
    
    virtual String GetExtension(core::StringView filePath) override;
    virtual String GetFilename(core::StringView filePath, bool hasExtension = true) override;
    virtual String GetDirectory(core::StringView filePath) override;

    virtual bool MakeDirectory(core::StringView directoryPath) override;

    virtual bool CheckFileExists(core::StringView filePath) override;
    virtual bool CheckDirectoryExists(core::StringView directoryPath) override;

    virtual uint32_t EnumrateFiles(core::StringView rootPath, TVector<String>& enumrateFiles, uint32_t recursive = 100) override;
    virtual uint32_t EnumrateFiles(const TVector<core::StringView>& rootPaths, TVector<String>& enumrateFiles, uint32_t recursive = 100) override;

    virtual uint32_t EnumrateFiles(core::StringView rootPath, core::StringView searchFileName, TVector<String>& enumrateFiles, uint32_t recursive = 100) override;
    virtual uint32_t EnumrateFiles(const TVector<core::StringView>& rootPaths, core::StringView searchFileName, TVector<String>& enumrateFiles, uint32_t recursive = 100) override;
    virtual uint32_t EnumrateFiles(const TVector<core::StringView>& rootPaths, const TVector<core::StringView>& searchFileNames, TVector<String>& enumrateFiles, uint32_t recursive = 100) override;

    virtual uint32_t EnumrateFilesExtension(core::StringView rootPath, core::StringView extension, TVector<String>& enumrateFiles, uint32_t recursive = 100) override;
    virtual uint32_t EnumrateFilesExtension(const TVector<core::StringView>& rootPaths, core::StringView extension, TVector<String>& enumrateFiles, uint32_t recursive = 100) override;
    virtual uint32_t EnumrateFilesExtension(const TVector<core::StringView>& rootPaths, const TVector<core::StringView>& extensions, TVector<String>& enumrateFiles, uint32_t recursive = 100) override;

    virtual String GetRootPath(core::StringView searchPath = ".root") override;
};
}
