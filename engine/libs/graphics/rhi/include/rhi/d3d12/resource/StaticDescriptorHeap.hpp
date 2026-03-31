#pragma once

#include "core/thread/Mutex.hpp"
#include "rhi/d3d12/interface/IDescriptorHeap.hpp"

namespace slug
{
namespace rhi
{
namespace d3d12
{

class Context;

class StaticDescriptorHeap : public rhi::d3d12::IDescriptorHeap {
public:
    explicit StaticDescriptorHeap(const Context& context);

    HRESULT AllocateResources(D3D12_DESCRIPTOR_HEAP_TYPE heap_type, uint32_t num_descriptors, bool shader_visible);
    void CopyToShaderVisibleHeap(rhi::d3d12::DescriptorIndex index, uint32_t count = 1);
    rhi::d3d12::DescriptorIndex AllocateDescriptors(uint32_t count) override;
    rhi::d3d12::DescriptorIndex AllocateDescriptor() override;

    void ReleaseDescriptors(rhi::d3d12::DescriptorIndex baseIndex, uint32_t count) override;
    void ReleaseDescriptor(rhi::d3d12::DescriptorIndex index) override;

    D3D12_CPU_DESCRIPTOR_HANDLE GetCpuHandle(rhi::d3d12::DescriptorIndex index) override;
    D3D12_CPU_DESCRIPTOR_HANDLE GetCpuHandleShaderVisible(rhi::d3d12::DescriptorIndex index) override;
    D3D12_GPU_DESCRIPTOR_HANDLE GetGpuHandle(rhi::d3d12::DescriptorIndex index) override;
    ID3D12DescriptorHeap* GetHeap() const override;
    ID3D12DescriptorHeap* GetShaderVisibleHeap() const override;
private:
    HRESULT Grow(uint32_t min_required_size);

private:
    const Context& m_context;
    core::TReferencePtr<ID3D12DescriptorHeap> m_heap;
    core::TReferencePtr<ID3D12DescriptorHeap> m_shaderVisibleHeap;
    D3D12_DESCRIPTOR_HEAP_TYPE m_heapType = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    D3D12_CPU_DESCRIPTOR_HANDLE m_startCpuHandle = { 0 };
    D3D12_CPU_DESCRIPTOR_HANDLE m_startCpuHandleShaderVisible = { 0 };
    D3D12_GPU_DESCRIPTOR_HANDLE m_startGpuHandleShaderVisible = { 0 };
    uint32_t m_stride = 0;
    uint32_t m_numDescriptors = 0;
    core::TVector<bool> m_allocatedDescriptors;
    rhi::d3d12::DescriptorIndex m_searchStart = 0;
    uint32_t m_numAllocatedDescriptors = 0;
    core::Mutex m_mutex;
};

}
}
}
