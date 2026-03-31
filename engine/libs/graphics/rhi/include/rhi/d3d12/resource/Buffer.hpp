#pragma once

#include "rhi/interface/IBuffer.hpp"
#include "rhi/interface/IHeap.hpp"
#include "rhi/common/StateTracking.hpp"
#include "rhi/d3d12/D3D12.hpp"

namespace slug
{
namespace rhi
{
namespace d3d12
{

class Context;
class DeviceResources;

class Buffer : public IBuffer, public BufferStateExtension
{
public:
    Buffer(const Context& context, DeviceResources& resources, BufferDesc desc);
    virtual ~Buffer() override;
    const BufferDesc& GetDesc() const override;
    Object GetNativeObject(ObjectType objectType) const;
    void PostCreate();
    DescriptorIndex GetClearUAV();
    void CreateCBV(size_t descriptor, BufferRange range) const;
    void CreateSRV(size_t descriptor, FormatType format, BufferRange range, ResourceType type) const;
    void CreateUAV(size_t descriptor, FormatType format, BufferRange range, ResourceType type) const;
    static void CreateNullSRV(size_t descriptor, FormatType foramt, const Context& context);
    static void CreateNullUAV(size_t descriptor, FormatType foramt, const Context& context);

public:
    const BufferDesc desc;
    core::TReferencePtr<ID3D12Resource> resource;
    D3D12_GPU_VIRTUAL_ADDRESS gpuVirtualAddress = {};
    D3D12_RESOURCE_DESC resourceDesc = {};
    HeapPtr heap = nullptr;
    core::TReferencePtr<ID3D12Fence> lastUseFence = nullptr;
    uint32_t lastUseFenceValue = 0;
    HANDLE sharedHandle = nullptr;

private:
    const Context& m_context;
    DeviceResources& m_resources;
    DescriptorIndex m_clearUav = INVALID_DESCRIPTOR_INDEX;
};
}
}
}
