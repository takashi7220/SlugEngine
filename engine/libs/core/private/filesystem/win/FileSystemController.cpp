#if defined(PLATFORM_WINDOWS)
#include "core/filesystem/win/FileSystemController.hpp"
#include "core/filesystem/win/FileHandle.hpp"
#include "core/io/Path.hpp"
#include <fstream>
#include <Windows.h>

namespace slug::core::win
{
slug::core::String FileSystemController::GetCurrentDirectoryImpl()
{
    char cdir[255];
    GetCurrentDirectory(255, cdir);
    return String(cdir);
}

FileHandlePtr FileSystemController::Open(const FileOepnDesc& desc)
{
    DWORD flags = FILE_ATTRIBUTE_NORMAL;

    if (desc.async)
    {
        flags |= FILE_FLAG_OVERLAPPED;
    }

    if (desc.sequential)
    {
        flags |= FILE_FLAG_SEQUENTIAL_SCAN;
    }

    if (desc.randomAccess)
    {
        flags |= FILE_FLAG_RANDOM_ACCESS;
    }

    core::WString tmp = core::StringUtility::GetWstring(desc.path.data());
    HANDLE handle = ::CreateFileW(tmp.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, flags, nullptr);
    if (handle == INVALID_HANDLE_VALUE)
    {
        return nullptr;
    }

    return core::MakeReference<FileHandle>(handle);
}

FileBlobPtr FileSystemController::Read(core::StringView filePath)
{

    size_t fileSize = 0;
    if (!Read(filePath, fileSize, nullptr))
    {
        return nullptr;
    }

    if (fileSize == 0)
    {
        return nullptr;
    }

    FileBlobPtr ret = NewObject<FileBlob>();
    ret->GetData().resize(fileSize);
    if (Read(filePath, fileSize, ret->GetData().data()))
    {
        return nullptr;
    }

    return ret;
}

bool FileSystemController::Read(core::StringView filePath, size_t& size, void* data)
{
    std::ifstream inFile(filePath.data(), std::ios::binary | std::ios::ate);
    if (!inFile)
    {
        size = 0;
        return false;
    }

    const std::streampos endPos = inFile.tellg();
    if (endPos < 0)
    {
        size = 0;
        return false;
    }

    const size_t fileSize = static_cast<size_t>(endPos);
    if (data == nullptr)
    {
        size = fileSize;
        return true;
    }

    if (size < fileSize)
    {
        size = fileSize;
        return false;
    }

    inFile.seekg(0, std::ios::beg);
    inFile.read(static_cast<char*>(data), static_cast<std::streamsize>(fileSize));

    if (!inFile && inFile.gcount() != static_cast<std::streamsize>(fileSize))
    {
        size = 0;
        return false;
    }

    size = fileSize;
    return true;
}

String FileSystemController::ReadText(core::StringView filePath)
{
    String text;
    ReadText(filePath, text);
    return text;
}

bool FileSystemController::ReadText(core::StringView filePath, String& out)
{
    out.clear();

    std::ifstream inFile(filePath.data(), std::ios::in | std::ios::binary | std::ios::ate);
    if (!inFile)
    {
        return false;
    }

    const std::streampos endPos = inFile.tellg();
    if (endPos < 0)
    {
        return false;
    }

    const size_t fileSize = static_cast<size_t>(endPos);

    out.resize(fileSize);

    inFile.seekg(0, std::ios::beg);

    if (fileSize > 0)
    {
        inFile.read(out.data(), static_cast<std::streamsize>(fileSize));

        if (!inFile)
        {
            out.clear();
            return false;
        }
    }

    return true;
}

bool FileSystemController::Write(core::StringView filePath, const FileBlobPtr& data, bool makeDir)
{
    return Write(filePath, data->GetData().size(), data->GetData().data(), makeDir);
}

bool FileSystemController::Write(core::StringView filePath, size_t size, void* data, bool makeDir)
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

bool FileSystemController::WriteText(core::StringView filePath, const core::String& text, bool makeDir)
{
    if (filePath.empty())
    {
        return false;
    }

    namespace fs = std::filesystem;

    const fs::path path(filePath.data());

    if (makeDir)
    {
        const fs::path parentPath = path.parent_path();

        if (!parentPath.empty())
        {
            std::error_code ec;
            fs::create_directories(parentPath, ec);

            if (ec)
            {
                return false;
            }
        }
    }

    std::ofstream outFile(path, std::ios::out | std::ios::binary | std::ios::trunc);
    if (!outFile)
    {
        return false;
    }

    if (!text.empty())
    {
        outFile.write(text.data(), static_cast<std::streamsize>(text.size()));

        if (!outFile)
        {
            return false;
        }
    }

    return true;
}

String FileSystemController::GetExtension(core::StringView filePath)
{
    return String(Path(filePath).extension().string().c_str());
}

String FileSystemController::GetFilename(core::StringView filePath, bool hasExtension)
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

String FileSystemController::GetDirectory(core::StringView filePath)
{
    Path tmpPath(filePath);
    return String(tmpPath.parent_path().string().c_str());
}

bool FileSystemController::MakeDirectory(core::StringView directoryPath)
{
    return PathUtility::CreateDirectories(Path(directoryPath));
}

bool FileSystemController::CheckFileExists(core::StringView filePath)
{
    return PathUtility::CheckFileExists(Path(filePath));
}

bool FileSystemController::CheckDirectoryExists(core::StringView directoryPath)
{
    return PathUtility::CheckDirectory(Path(directoryPath));
}

uint32_t FileSystemController::EnumrateFiles(core::StringView rootPath, TVector<String>& enumrateFiles, uint32_t recursive)
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

uint32_t FileSystemController::EnumrateFiles(const TVector<core::StringView>& rootPaths, TVector<String>& enumrateFiles, uint32_t recursive)
{
    for (auto& rootPath : rootPaths)
    {
        EnumrateFiles(rootPath, enumrateFiles, recursive);
    }
    return static_cast<uint32_t>(enumrateFiles.size());
}

uint32_t FileSystemController::EnumrateFiles(core::StringView rootPath, core::StringView searchFileName, TVector<String>& enumrateFiles, uint32_t recursive)
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

uint32_t FileSystemController::EnumrateFiles(const TVector<core::StringView>& rootPaths, core::StringView searchFileName, TVector<String>& enumrateFiles, uint32_t recursive)
{
    for(auto& rootPath : rootPaths)
    {
        EnumrateFiles(rootPath, searchFileName, enumrateFiles, recursive);
    }
    return static_cast<uint32_t>(enumrateFiles.size());
}

uint32_t FileSystemController::EnumrateFiles(const TVector<core::StringView>& rootPaths, const TVector<core::StringView>& searchFileNames, TVector<String>& enumrateFiles, uint32_t recursive)
{
    for (auto& fileName : searchFileNames)
    {
        EnumrateFiles(rootPaths, fileName, enumrateFiles, recursive);
    }
    return static_cast<uint32_t>(enumrateFiles.size());
}

uint32_t FileSystemController::EnumrateFilesExtension(core::StringView rootPath, core::StringView searchExtension, TVector<String>& enumrateFiles, uint32_t recursive)
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

uint32_t FileSystemController::EnumrateFilesExtension(const TVector<core::StringView>& rootPaths, core::StringView searchExtension, TVector<String>& enumrateFiles, uint32_t recursive)
{
    for (auto& rootPath : rootPaths)
    {
        EnumrateFilesExtension(rootPath, searchExtension, enumrateFiles, recursive);
    }
    return static_cast<uint32_t>(enumrateFiles.size());
}

uint32_t FileSystemController::EnumrateFilesExtension(const TVector<core::StringView>& rootPaths, const TVector<core::StringView>& searchExtensions, TVector<String>& enumrateFiles, uint32_t recursive)
{
    for (auto& extension : searchExtensions)
    {
        EnumrateFilesExtension(rootPaths, extension, enumrateFiles, recursive);
    }
    return static_cast<uint32_t>(enumrateFiles.size());
}

String FileSystemController::GetRootPath(core::StringView rootPath)
{
    Path currentPath(GetCurrentDirectoryImpl());
    while (currentPath.has_parent_path()) 
    {
        Path filePath = currentPath / rootPath;
        if (CheckFileExists(filePath.string().c_str()) || CheckDirectoryExists(filePath.string().c_str())) 
        {
            return String(currentPath.string().c_str());
        }
        currentPath = currentPath.parent_path();
    }
    return GetCurrentDirectoryImpl();
}
}
#endif
