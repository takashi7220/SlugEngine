#pragma once

#include "rhi/interface/IHeap.hpp"
#include "rhi/common/StateTracking.hpp"
#include "rhi/d3d12/resource/DeviceResources.hpp"

namespace slug::rhi::d3d12
{

class Heap : public IHeap
{
public:
    const HeapDesc& GetDesc() override
    {
        return desc;
    }
public:
    HeapDesc desc;
    core::TReferencePtr<ID3D12Heap> heap;
};

}
