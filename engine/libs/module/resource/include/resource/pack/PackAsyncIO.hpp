#pragma once

#include <cstdint>
#include "core/filesystem/FileSystem.hpp"
#include "core/task/TaskSystem.hpp"

namespace slug::resource
{

using IOHandle = uint64_t;
static constexpr IOHandle s_InvalidIOHandle = 0;

enum class IORequestPriority : uint8_t
{
    Critical,
    Hight,
    Normal,
    Low,
    Background
};

enum class IOStatus : uint8_t
{
    Pending,
    Completed,
    Canceled,
    Failed
};

struct IOResult
{
    IOHandle handle = s_InvalidIOHandle;
    IOStatus status = IOStatus::Pending;
    uint32_t bytesRead = 0;
};

struct IORequest
{
    core::FileHandlePtr file;
    uint64_t offset = 0;
    uint32_t size = 0;
    void* dst = nullptr;
    uint32_t dstSize = 0;
    IORequestPriority priority = IORequestPriority::Normal;
    uint64_t cancelTag = 0;
};

class PackAsyncIO
{
public:
    IOHandle ReadAsync(const IORequest& request, core::TaskSystem* taskSystem, IOResult& result);
#if 0
    bool Poll(IOHandle handle);
    void Cancel(IOHandle handle);
    void CancelByTag(uint64_t cancelTag);
#endif

private:
    core::TAtomic<IOHandle> m_nextHandle = 1;
};
}
