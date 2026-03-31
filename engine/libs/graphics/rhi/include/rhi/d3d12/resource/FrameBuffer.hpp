#pragma once

#include "rhi/d3d12/resource/FrameBuffer.hpp"
#include "rhi/d3d12/resource/DeviceResources.hpp"
#include "rhi/d3d12/D3D12.hpp"

namespace slug
{
namespace rhi
{
namespace d3d12
{

class DeviceResources;

class FrameBuffer : public IFrameBuffer
{
public:
    FrameBuffer(DeviceResources& resources);
    ~FrameBuffer() override;
    const FrameBufferDesc& GetDesc() const override;
    const FrameBufferInfoEx& GetFrameBufferInfo() const override;
    FrameBufferInfoEx& GetFrameBufferInfo() override;

public:
    FrameBufferDesc desc;
    FrameBufferInfoEx frameBufferInfo;
    core::TStaticVector<TexturePtr, MAX_RENDER_TARGETS + 1> textures;
    core::TStaticVector<DescriptorIndex, MAX_RENDER_TARGETS> rtvs;
    DescriptorIndex dsv = INVALID_DESCRIPTOR_INDEX;
    uint32_t rtWidth = 0;
    uint32_t rtHeight = 0;

private:
    DeviceResources& m_resources;
};

}
}
}
