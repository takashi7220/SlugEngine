#pragma once

namespace slug
{
namespace rhi
{
namespace d3d12
{

class EventQuery : public IEventQuery
{
public:
    core::TReferencePtr<ID3D12Fence> fence;
    uint64_t fenceCounter = 0;
    bool started = false;
    bool resolved = false;
};

}
}
}
