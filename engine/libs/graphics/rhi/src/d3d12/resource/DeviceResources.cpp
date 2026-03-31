#include "rhi/d3d12/resource/DeviceResources.hpp"
#include "rhi/d3d12/resource/Device.hpp"
#include "rhi/d3d12/resource/RootSignature.hpp"
#include "rhi/d3d12/resource/Context.hpp"

namespace slug
{
namespace rhi
{
namespace d3d12
{
DeviceResources::DeviceResources(const Context& context, const DeviceDesc& desc)
    : renderTargetViewHeap(context)
    , depthStencilViewHeap(context)
    , shaderResourceViewHeap(context)
    , samplerHeap(context)
    , timerQueries(desc.maxTimerQueries, true)
    , m_context(context)

{
}

uint32_t DeviceResources::GetFormatPlaneCount(DXGI_FORMAT format) 
{
    uint32_t planeCount = 0;
    auto planeCountItr = m_dxgiFormatPlaneCounts.find(format);
    if (planeCountItr != m_dxgiFormatPlaneCounts.end()) 
    {
        planeCount = planeCountItr->second;
    }

    if (planeCount == 0) 
    {
        D3D12_FEATURE_DATA_FORMAT_INFO formatInfo = { format, 1 };
        HRESULT hr = m_context.device->CheckFeatureSupport(D3D12_FEATURE_FORMAT_INFO, &formatInfo, sizeof(formatInfo));
        if (FAILED(hr)) 
        {
            planeCount = 255;
        }
        else 
        {
            planeCount = formatInfo.PlaneCount;
        }
    }

    if (planeCount == 255) 
    {
        return 0;
    }

    return planeCount;
}
}
}
}
