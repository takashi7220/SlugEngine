#pragma once

#include "core/container/Map.hpp"
#include "rhi/common/BitsetAllocator.hpp"
#include "rhi/d3d12/interface/IDevice.hpp"
#include "rhi/d3d12/interface/IDescriptorHeap.hpp"
#include "rhi/d3d12/resource/StaticDescriptorHeap.hpp"

namespace slug::rhi::d3d12
{
class RootSignature;
class Context;

class DeviceResources {
public:
    explicit DeviceResources(const Context& context, const DeviceDesc& desc);
    uint32_t GetFormatPlaneCount(DXGI_FORMAT format);

public:
    StaticDescriptorHeap renderTargetViewHeap;
    StaticDescriptorHeap depthStencilViewHeap; 
    StaticDescriptorHeap shaderResourceViewHeap;
    StaticDescriptorHeap samplerHeap;
    rhi::BitsetAllocator timerQueries; 
    core::TUnorderedMap<size_t, RootSignature*> rootsignatureCache;

private:
    const Context& m_context;
    core::TUnorderedMap<DXGI_FORMAT, uint32_t> m_dxgiFormatPlaneCounts;
};
}
