#pragma once

#include <d3d12.h>
#include "core/container/Vector.hpp"
#include "core/container/StaticVector.hpp"
#include "rhi/d3d12/D3D12.hpp"
#include "rhi/interface/IBindingLayout.hpp"

namespace slug 
{
namespace rhi 
{
namespace d3d12 
{
class BindingLayout : public IBindingLayout
{
public:
    BindingLayout(const BindingLayoutDesc& desc);
    const BindingLayoutDesc* GetDesc() const override
    {
        return &desc;
    }
    const BindlessLayoutDesc* GetBindlessDesc() const override
    {
        return nullptr;
    }
public:
    BindingLayoutDesc desc;
    uint32_t pushConstantByteSize = 0;
    RootParameterIndex rootParameterPushConstants = ~0u;
    RootParameterIndex rootParameterSrvEtc = ~0u;
    RootParameterIndex rootParameterSamplers = ~0u;
    int32_t descriptorTableSizeSrvEtc = 0;
    int32_t descriptorTableSizeSamplers = 0;
    core::TVector<D3D12_DESCRIPTOR_RANGE1> descriptorRangesSrvEtc;
    core::TVector<D3D12_DESCRIPTOR_RANGE1> descriptorRangesSamplers;
    core::TVector<BindingLayoutItem> bindingLayoutsSrvEtc;
    core::TStaticVector<core::TPair<RootParameterIndex, D3D12_ROOT_DESCRIPTOR1>, MAX_VOLATILE_CONSTANT_BUFFERS_PER_LAYOUT> rootParametersVolatileCb;
    core::TStaticVector<D3D12_ROOT_PARAMETER1, 32> rootParameters;
};
}
}
}
