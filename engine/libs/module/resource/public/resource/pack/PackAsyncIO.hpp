#pragma once

#include <cstdint>
#include "core/filesystem/FileSystem.hpp"
#include "core/task/TaskSystem.hpp"
#include "core/memory/ReferencePtr.hpp"

namespace slug::resource
{
enum class IOStatus : uint8_t
{
    Pending,
    Completed,
    Canceled,
    Failed
};

struct IOResult
{
    IOStatus status = IOStatus::Pending;
    uint32_t bytesRead = 0;
};

// タスク完了後も生存できるよう、結果は呼び出し元のスタックではなく参照カウントで共有する。
class IOResultBox : public core::ReferenceObject
{
public:
    IOResult result;
};
using IOResultPtr = core::TReferencePtr<IOResultBox>;

struct IORequest
{
    core::FileHandlePtr file;
    uint64_t offset = 0;
    uint32_t size = 0;
    void* dst = nullptr;
    uint32_t dstSize = 0;
};

struct PackReadHandle
{
    core::TaskHandle task;
    IOResultPtr result;

    bool Valid() const noexcept
    {
        return task.Valid() && result != nullptr;
    }
};

class PackAsyncIO
{
public:
    PackReadHandle ReadAsync(const IORequest& request, core::TaskSystem* taskSystem);
};
}
