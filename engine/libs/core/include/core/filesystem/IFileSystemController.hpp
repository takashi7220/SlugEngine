#pragma once

#include "core/container/Vector.hpp"
#include "core/container/String.hpp"
#include "core/memory/ReferencePtr.hpp"

namespace slug::core
{

class FileBlob : public ReferenceObject
{
public:
    explicit FileBlob(void* data, size_t size) 
        : m_data(data)
        , m_size(size)
    {}

    virtual ~FileBlob() 
    {
        if (m_data)
        {
            delete m_data;
            m_data = nullptr;
        }
        m_size = 0;

    }

    void* data()
    {
        return m_data;
    }

    const size_t size() const 
    {
        return m_size;
    }
private:
    size_t m_size = 0;
    void* m_data = {};
};
using FileBlobPtr = TReferencePtr<FileBlob>;

class IFileSystemController 
{
public:
    virtual FileBlobPtr Read(core::StringView filePath) = 0;
    virtual bool Read(core::StringView filePath, size_t& size, void* data) = 0;

    virtual bool Write(core::StringView filePath, const FileBlobPtr& data, bool makeDir = true) = 0;
    virtual bool Write(core::StringView filePath, size_t size, void* data, bool makeDir = true) = 0;

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
