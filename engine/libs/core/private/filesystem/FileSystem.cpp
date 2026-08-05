#include "core/filesystem/FileSystem.hpp"
#include "core/memory/UniquePtr.hpp"

#if defined(PLATFORM_WINDOWS)
#include "core/filesystem/win/FileSystemController.hpp"
#endif

namespace slug::core
{

FileHandlePtr FileSystem::Open(const FileOepnDesc& desc)
{
    return GetController()->Open(desc);
}

FileBlobPtr FileSystem::Read(core::StringView filePath) 
{
    return GetController()->Read(filePath);
}

bool FileSystem::Read(core::StringView filePath, size_t& size, void* data) 
{
    return GetController()->Read(filePath, size, data);
}

String FileSystem::ReadText(core::StringView filePath)
{
    return GetController()->ReadText(filePath);
}

bool FileSystem::ReadText(core::StringView filePath, String& out)
{
    return GetController()->ReadText(filePath, out);
}

bool FileSystem::Write(core::StringView filePath, const FileBlobPtr& data, bool makeDir)
{
    return GetController()->Write(filePath, data, makeDir);
}

bool FileSystem::Write(core::StringView filePath, size_t size, void* data, bool makeDir)
{
    return GetController()->Write(filePath, size, data, makeDir);
}

bool FileSystem::WriteText(core::StringView filePath, const String& text, bool makeDir)
{
    return GetController()->WriteText(filePath, text, makeDir);
}

bool FileSystem::TryGetLines(core::StringView filePath, core::TVector<String>& lines)
{
    lines.clear();

    core::String text;
    if (!ReadText(filePath, text))
    {
        return false;
    }

    return TryGetLines(text, lines);
}

bool FileSystem::TryGetLines(const core::String& text, core::TVector<String>& lines)
{
    lines.clear();

    String line;

    for (size_t i = 0; i < text.size(); ++i)
    {
        const char c = text[i];

        if (c == '\r')
        {
            if ((i + 1) < text.size() && text[i + 1] == '\n')
            {
                ++i;
            }

            lines.push_back(line);
            line.clear();
        }
        else if (c == '\n')
        {
            lines.push_back(line);
            line.clear();
        }
        else
        {
            line.push_back(c);
        }
    }

    if (!line.empty())
    {
        lines.push_back(line);
    }

    return true;
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

void FileSystem::BindController(IFileSystemController* controller)
{
    GetRegistry().controller = controller;
}

void FileSystem::UnbindController()
{
    GetRegistry().controller = nullptr;
}

FileSystem::Registry& FileSystem::GetRegistry()
{
    static Registry registry;
    if (registry.controller == nullptr)
    {
        BindControllerInternal();
    }
    return registry;
}

IFileSystemController* FileSystem::GetController()
{
    return GetRegistry().controller;
}

void FileSystem::BindControllerInternal()
{
#if defined(PLATFORM_WINDOWS)
    static win::FileSystemController controller;
    BindController(&controller);
#endif
}
}
