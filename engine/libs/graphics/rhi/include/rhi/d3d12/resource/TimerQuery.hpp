#pragma once

#include "rhi/interface/ITimerQuery.hpp"
#include "rhi/d3d12/resource/DeviceResources.hpp"

namespace slug
{
namespace rhi
{
namespace d3d12
{

class TimerQuery : public ITimerQuery
{
public:
    TimerQuery(DeviceResources& resources);
    ~TimerQuery() override;

public:
    core::TReferencePtr<ID3D12Fence> fence;
    uint32_t beginQueryIndex = 0;
    uint32_t endQueryIndex = 0;
    uint64_t fenceCounter = 0;
    bool started = false;
    bool resolved = false;
    float time = 0.f;

private:
    DeviceResources& m_resources;
};

}
}
}
