#pragma once

#include <d3d12.h>
#include "rhi/ResourceType.hpp"
#include "rhi/interface/IBuffer.hpp"
#include "rhi/interface/ICommandList.hpp"

namespace slug 
{
namespace rhi 
{
namespace d3d12 
{
using DescriptorIndex = uint32_t;

class IDescriptorHeap {
protected:
    IDescriptorHeap() = default;
    virtual ~IDescriptorHeap() = default;
public:
    virtual DescriptorIndex AllocateDescriptors(uint32_t count) = 0;
    virtual DescriptorIndex AllocateDescriptor() = 0;
    virtual void ReleaseDescriptors(DescriptorIndex baseIndex, uint32_t count) = 0;
    virtual void ReleaseDescriptor(DescriptorIndex index) = 0;
    virtual D3D12_CPU_DESCRIPTOR_HANDLE GetCpuHandle(DescriptorIndex index) = 0;
    virtual D3D12_CPU_DESCRIPTOR_HANDLE GetCpuHandleShaderVisible(DescriptorIndex index) = 0;
    virtual D3D12_GPU_DESCRIPTOR_HANDLE GetGpuHandle(DescriptorIndex index) = 0;
    virtual ID3D12DescriptorHeap* GetHeap() const = 0;
    virtual ID3D12DescriptorHeap* GetShaderVisibleHeap() const = 0;

    IDescriptorHeap(const IDescriptorHeap&) = delete;
    IDescriptorHeap(const IDescriptorHeap&&) = delete;
    IDescriptorHeap& operator=(const IDescriptorHeap&) = delete;
    IDescriptorHeap& operator=(const IDescriptorHeap&&) = delete;
};

}
}
}