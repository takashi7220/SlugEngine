#include "core/filesystem/FileMapping.hpp"

#if PLATFORM_WINDOWS
#include <Windows.h>
#endif

namespace slug
{
namespace core
{

#if PLATFORM_WINDOWS
struct FileMappingObject::Implement : core::MemoryObject
{
    void* fileHandle = nullptr;
    void* mappingHandle = nullptr;
    void* data = nullptr;
    size_t size;

    void Open(core::StringView filePath)
    {
        fileHandle = CreateFileA(filePath.data(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (fileHandle == INVALID_HANDLE_VALUE)
        {
            return;
        }

        LARGE_INTEGER fileSize;
        if (!GetFileSizeEx(fileHandle, &fileSize))
        {
            return;
        }
        size = static_cast<size_t>(fileSize.QuadPart);

        mappingHandle = CreateFileMappingA(fileHandle, nullptr, PAGE_READONLY, 0, 0, nullptr);
        if (!mappingHandle)
        {
            return;
        }

        data = MapViewOfFile(mappingHandle, FILE_MAP_READ, 0, 0, 0);
    }

    void Close()
    {
        if (data)
        {
            UnmapViewOfFile(data);
        }

        if (mappingHandle)
        {
            CloseHandle(mappingHandle);
        }

        if (fileHandle)
        {
            CloseHandle(fileHandle);
        }
    }
};
#endif

FileMappingObject* FileMappingObject::Create(core::StringView filePath)
{
    return new FileMappingObject(filePath);
}

FileMappingObject::FileMappingObject(core::StringView filePath)
{
    m_impl.reset(new Implement());
    Open(filePath);
}

FileMappingObject::~FileMappingObject()
{
    Close();
}

void FileMappingObject::Open(core::StringView filePath)
{
    m_impl->Open(filePath);
}

void FileMappingObject::Close()
{
    m_impl->Close();
}

void* FileMappingObject::GetData()
{
    return m_impl->data;
}

size_t FileMappingObject::GetSize()
{
    return m_impl->size;
}

}
}
