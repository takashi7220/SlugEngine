#pragma once

#include "rhi/Rhi.hpp"
#include "rhi/d3d12/D3D12.hpp"
#include "rhi/d3d12/resource/BindingLayout.hpp"

namespace slug::rhi::d3d12
{
class BindlessLayout : public IBindingLayout
{
public:
    BindlessLayout(const BindlessLayoutDesc& desc);
    const BindingLayoutDesc* GetDesc() const override
    {
        return nullptr;
    }
    const BindlessLayoutDesc* GetBindlessDesc() const override
    {
        return &desc;
    }
public:
    BindlessLayoutDesc desc;
    core::TStaticVector<D3D12_DESCRIPTOR_RANGE1, 32> descriptorRanges;
    D3D12_ROOT_PARAMETER1 rootParameter {};
};
}
