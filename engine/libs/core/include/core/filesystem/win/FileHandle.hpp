#pragma once

#if defined(PLATFORM_WINDOWS)
#include "core/filesystem/IFileHandle.hpp"
#include <Windows.h>

namespace slug::core::win
{

class FileHandle : public IFileHandle
{
public:
    explicit FileHandle(HANDLE handle);
    ~FileHandle() override;

    bool IsValid() const override;
    void Close()  override;
    uint64_t GetSize() const override;
    bool ReadAt(uint64_t offset, void* dst, uint32_t size) override;

    HANDLE GetNativeHandle() const;
private:
    HANDLE m_handle = INVALID_HANDLE_VALUE;
};
}
#endif
