#pragma once

#include "rhi/d3d12/D3D12.hpp"
#include "rhi/d3d12/resource/StaticDescriptorHeap.hpp"
#include "rhi/d3d12/resource/Context.hpp"

namespace slug::rhi::d3d12
{

StaticDescriptorHeap::StaticDescriptorHeap(const Context& context)
    :m_context(context)
{
}

HRESULT StaticDescriptorHeap::AllocateResources(D3D12_DESCRIPTOR_HEAP_TYPE heapType, uint32_t numDescriptors, bool shaderVisible)
{
    m_heap = nullptr;
    m_shaderVisibleHeap = nullptr;

    D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
    heapDesc.Type = heapType;
    heapDesc.NumDescriptors = numDescriptors;
    heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

    HRESULT hr = m_context.device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_heap));

    if (FAILED(hr))
    {
        return hr;
    }

    if (shaderVisible)
    {
        heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

        hr = m_context.device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_shaderVisibleHeap));

        if (FAILED(hr)) {
            return hr;
        }

        m_startCpuHandleShaderVisible = m_shaderVisibleHeap->GetCPUDescriptorHandleForHeapStart();
        m_startGpuHandleShaderVisible = m_shaderVisibleHeap->GetGPUDescriptorHandleForHeapStart();
    }

    m_numDescriptors = heapDesc.NumDescriptors;
    m_heapType = heapDesc.Type;
    m_startCpuHandle = m_heap->GetCPUDescriptorHandleForHeapStart();
    m_stride = m_context.device->GetDescriptorHandleIncrementSize(heapDesc.Type);
    m_allocatedDescriptors.resize(m_numDescriptors);

    return S_OK;
}

void StaticDescriptorHeap::CopyToShaderVisibleHeap(rhi::d3d12::DescriptorIndex index, uint32_t count)
{
    m_context.device->CopyDescriptorsSimple(count, GetCpuHandleShaderVisible(index), GetCpuHandle(index), m_heapType);
}

rhi::d3d12::DescriptorIndex StaticDescriptorHeap::AllocateDescriptors(uint32_t count)
{
    core::LockGuard<core::Mutex> lockGuard(m_mutex);

    rhi::d3d12::DescriptorIndex foundIndex = 0;
    uint32_t freeCount = 0;
    bool found = false;
    for (DescriptorIndex index = m_searchStart; index < m_numDescriptors; index++)
    {
        if (m_allocatedDescriptors[index])
        {
            freeCount = 0;
        }
        else
        {
            freeCount += 1;
        }

        if (freeCount >= count) 
        {
            foundIndex = index - count + 1;
            found = true;
            break;
        }
    }

    if (!found) 
    {
        foundIndex = m_numDescriptors;

        if (FAILED(Grow(m_numDescriptors + count))) 
        {
            SLUG_LOG(D3D12LogCategory, core::LogType::Error, "Failed to grow a descriptor heap!");
            return ~0u;
        }
    }

    for (DescriptorIndex index = foundIndex; index < foundIndex + count; index++) 
    {
        m_allocatedDescriptors[index] = true;
    }

    m_numAllocatedDescriptors += count;

    m_searchStart = foundIndex + count;
    return foundIndex;
}

rhi::d3d12::DescriptorIndex StaticDescriptorHeap::AllocateDescriptor()
{
    return AllocateDescriptors(1);
}

void StaticDescriptorHeap::ReleaseDescriptors(rhi::d3d12::DescriptorIndex baseIndex, uint32_t count)
{
    core::LockGuard<core::Mutex> lockGuard(m_mutex);

    if (count == 0) 
    {
        return;
    }

    for (DescriptorIndex index = baseIndex; index < baseIndex + count; index++) 
    {
#ifdef DEBUG_MODE
        if (!m_allocatedDescriptors[index]) 
        {
            SLUG_LOG(D3D12LogCategory, core::LogType::Error, "Failed to grow a descriptor heap!");
        }
#endif

        m_allocatedDescriptors[index] = false;
    }

    m_numAllocatedDescriptors -= count;

    if (m_searchStart > baseIndex)
    {
        m_searchStart = baseIndex;
    }
}

void StaticDescriptorHeap::ReleaseDescriptor(rhi::d3d12::DescriptorIndex index)
{
    ReleaseDescriptors(index, 1);
}

D3D12_CPU_DESCRIPTOR_HANDLE StaticDescriptorHeap::GetCpuHandle(rhi::d3d12::DescriptorIndex index)
{
    D3D12_CPU_DESCRIPTOR_HANDLE handle = m_startCpuHandle;
    handle.ptr += index * m_stride;
    return handle;
}

D3D12_CPU_DESCRIPTOR_HANDLE StaticDescriptorHeap::GetCpuHandleShaderVisible(rhi::d3d12::DescriptorIndex index)
{
    D3D12_CPU_DESCRIPTOR_HANDLE handle = m_startCpuHandleShaderVisible;
    handle.ptr += index * m_stride;
    return handle;
}

D3D12_GPU_DESCRIPTOR_HANDLE StaticDescriptorHeap::GetGpuHandle(rhi::d3d12::DescriptorIndex index)
{
    D3D12_GPU_DESCRIPTOR_HANDLE handle = m_startGpuHandleShaderVisible;
    handle.ptr += index * m_stride;
    return handle;
}

ID3D12DescriptorHeap* StaticDescriptorHeap::GetHeap() const
{
    return m_heap;
}

ID3D12DescriptorHeap* StaticDescriptorHeap::GetShaderVisibleHeap() const
{
    return m_shaderVisibleHeap;
}

inline uint32_t NextPowerOf2(uint32_t v)
{
    v--;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    v++;

    return v;
}

HRESULT StaticDescriptorHeap::Grow(uint32_t min_required_size)
{
    uint32_t oldSize = m_numDescriptors;
    uint32_t newSize = NextPowerOf2(min_required_size);

    core::TReferencePtr<ID3D12DescriptorHeap> oldHeap = m_heap;

    HRESULT hr = AllocateResources(m_heapType, newSize, m_shaderVisibleHeap != nullptr);

    if (FAILED(hr)) 
    {
        return hr;
    }

    m_context.device->CopyDescriptorsSimple(oldSize, m_startCpuHandle, oldHeap->GetCPUDescriptorHandleForHeapStart(), m_heapType);

    if (m_shaderVisibleHeap != nullptr) 
    {
        m_context.device->CopyDescriptorsSimple(oldSize, m_startCpuHandleShaderVisible, oldHeap->GetCPUDescriptorHandleForHeapStart(), m_heapType);
    }

    return S_OK;
}


}
