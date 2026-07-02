#include "resource/pack/PackAsyncIO.hpp"

namespace slug::resource
{
IOHandle PackAsyncIO::ReadAsync(const IORequest& request, core::TaskSystem* taskSystem, IOResult& result)
{
    if (!request.file
        || !request.file->IsValid()
        || request.dst == nullptr
        || request.size == 0
        || request.dstSize < request.size
        || taskSystem == nullptr)
    {
        return s_InvalidIOHandle;
    }

    const IOHandle handle = m_nextHandle.fetch_add(1, core::MemoryOrderRelaxed);
    core::TaskHandle taskHandle = taskSystem->Launch([handle, request, &result ]{
        result.handle = handle;

        if (!request.file->ReadAt(request.offset, request.dst, request.dstSize))
        {
            result.status = IOStatus::Failed;
            result.bytesRead = 0;
            return;
        }

        result.status = IOStatus::Completed;
        result.bytesRead = request.size;
    });
    return handle;
}
}

