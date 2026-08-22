#include "core/filesystem/win/FileHandle.hpp"

namespace slug::core::win
{

FileHandle::FileHandle(HANDLE handle)
    : IFileHandle()
    , m_handle(handle)
{

}

FileHandle::~FileHandle()
{
    Close();
}

bool FileHandle::IsValid() const
{
    return m_handle != INVALID_HANDLE_VALUE && m_handle != nullptr;
}

void FileHandle::Close()
{
    if (!IsValid())
    {
        return;
    }

    ::CloseHandle(m_handle);
    m_handle = INVALID_HANDLE_VALUE;
}

uint64_t FileHandle::GetSize() const
{
    if (!IsValid())
    {
        return 0;
    }

    LARGE_INTEGER size {};
    if (!::GetFileSizeEx(m_handle, &size))
    {
        return 0;
    }

    return static_cast<uint64_t>(size.QuadPart);
}

bool FileHandle::ReadAt(uint64_t offset, void* dst, uint32_t size)
{
    if (!IsValid() || dst == nullptr)
    {
        return false;
    }

    if (size == 0)
    {
        return true;
    }

    OVERLAPPED overlapped {};
    overlapped.Offset = static_cast<DWORD>(offset & 0xffffffff);
    overlapped.OffsetHigh = static_cast<DWORD>((offset >> 32) & 0xffffffff);

    DWORD bytesRead = 0;

    const BOOL result = ::ReadFile(m_handle, dst, size, &bytesRead, &overlapped);

    if (!result)
    {
        const DWORD error = ::GetLastError();
        if (error != ERROR_IO_PENDING)
        {
            return false;
        }

        if (!::GetOverlappedResult(m_handle, &overlapped, &bytesRead, TRUE))
        {
            return false;
        }
    }

    return bytesRead == size;
}

HANDLE FileHandle::GetNativeHandle() const
{
    return m_handle;
}

}
