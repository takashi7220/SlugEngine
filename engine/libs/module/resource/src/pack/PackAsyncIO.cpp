#include "resource/pack/PackAsyncIO.hpp"

namespace slug::resource
{
PackReadHandle PackAsyncIO::ReadAsync(const IORequest& request, core::TaskSystem* taskSystem)
{
    if (!request.file
        || !request.file->IsValid()
        || request.dst == nullptr
        || request.size == 0
        || request.dstSize < request.size
        || taskSystem == nullptr)
    {
        return {};
    }

    IOResultPtr resultPtr = core::MakeReference<IOResultBox>();

    core::TaskHandle taskHandle = taskSystem->Launch([request, resultPtr]{
        if (!request.file->ReadAt(request.offset, request.dst, request.dstSize))
        {
            resultPtr->result.status = IOStatus::Failed;
            resultPtr->result.bytesRead = 0;
            return;
        }

        resultPtr->result.status = IOStatus::Completed;
        resultPtr->result.bytesRead = request.size;
    });

    return PackReadHandle{ taskHandle, resultPtr };
}
}
