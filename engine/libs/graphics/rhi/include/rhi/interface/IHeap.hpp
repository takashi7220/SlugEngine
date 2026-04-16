#pragma once

#include "rhi/ResourceType.hpp"

namespace slug::rhi
{

struct HeapDesc
{
    uint64_t capacity = 0;
    HeapType type = HeapType::DeviceLocal;
    core::String debugName = "";
};

class IHeap : public IResource
{
public:
    virtual const HeapDesc& GetDesc() = 0;
};

using HeapPtr = core::TReferencePtr<IHeap>;

}