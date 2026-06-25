#pragma once

namespace slug::rhi::d3d12
{

class EventQuery : public IEventQuery
{
public:
    SLUG_DECLARE_SOBJECT_TYPEINFO(EventQuery, IEventQuery)
    core::TReferencePtr<ID3D12Fence> fence;
    uint64_t fenceCounter = 0;
    bool started = false;
    bool resolved = false;
};

}
