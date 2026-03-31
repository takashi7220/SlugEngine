#include "rhi/d3d12/resource/FrameBuffer.hpp"

namespace slug
{
namespace rhi
{
namespace d3d12
{
FrameBuffer::FrameBuffer(DeviceResources& resources)
    : m_resources(resources)
{
}

FrameBuffer::~FrameBuffer()
{
    for (DescriptorIndex rtv : rtvs)
    {
        m_resources.renderTargetViewHeap.ReleaseDescriptor(rtv);
    }

    if (dsv != INVALID_DESCRIPTOR_INDEX)
    {
        m_resources.depthStencilViewHeap.ReleaseDescriptor(dsv);
    }
}

const FrameBufferDesc& FrameBuffer::GetDesc() const
{
    return desc;
}

const FrameBufferInfoEx& FrameBuffer::GetFrameBufferInfo() const
{
    return frameBufferInfo;
}

FrameBufferInfoEx& FrameBuffer::GetFrameBufferInfo()
{
    return frameBufferInfo;
}


}
}
}
