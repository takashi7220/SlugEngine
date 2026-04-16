#include "core/memory/MemoryUtilities.hpp"
#include "rhi/d3d12/resource/BindingSet.hpp"
#include "rhi/d3d12/resource/Buffer.hpp"
#include "rhi/d3d12/resource/Sampler.hpp"
#include "rhi/d3d12/resource/Texture.hpp"
#include "rhi/d3d12/resource/AccelStruct.hpp"
#include "rhi/common/Utility.hpp"
#include "rhi/common/StateTracking.hpp"

namespace slug::rhi::d3d12
{

BindingSet::BindingSet(const Context& context, DeviceResources& resources)
    : m_context(context)
    , m_resources(resources)
{
}

BindingSet::~BindingSet()
{
    m_resources.shaderResourceViewHeap.ReleaseDescriptors(descriptorTableSrvEtc, layout->descriptorTableSizeSrvEtc);
    m_resources.samplerHeap.ReleaseDescriptors(descriptorTableSamplers, layout->descriptorTableSizeSamplers);
}

const BindingSetDesc* BindingSet::GetDesc() const
{
    return &desc;
}

IBindingLayout* BindingSet::GetLayout() const
{
    return layout;
}

void BindingSet::CreateDescriptors()
{
    for (const core::TPair<RootParameterIndex, D3D12_ROOT_DESCRIPTOR1>& parameter : layout->rootParametersVolatileCb)
    {
        IBuffer* foundBuffer = nullptr;
        RootParameterIndex rootParameterIndex = parameter.first;
        const D3D12_ROOT_DESCRIPTOR1& rootDescriptor = parameter.second;

        for (const auto& binding : desc.bindings)
        {
            if (binding.type == ResourceType::VolatileConstantBuffer && binding.slot == rootDescriptor.ShaderRegister)
            {
                Buffer* buffer = core::MemoryUtilities::CheckedCast<Buffer*>(binding.resourcePtr);
                resources.push_back(buffer);

                foundBuffer = buffer;
                break;
            }
        }

        rootParametersVolatileCb.push_back(core::MakePair(rootParameterIndex, foundBuffer));
    }

    if (layout->descriptorTableSizeSamplers > 0)
    {
        DescriptorIndex descriptorTableBaseIndex = m_resources.samplerHeap.AllocateDescriptors(layout->descriptorTableSizeSamplers);
        descriptorTableSamplers = descriptorTableBaseIndex;
        rootParameterIndexSamplers = layout->rootParameterSamplers;
        descriptorTableValidSamplers = true;

        for (const auto& range : layout->descriptorRangesSamplers)
        {
            for (uint32_t itemInRange = 0; itemInRange < range.NumDescriptors; itemInRange++)
            {
                uint32_t slot = range.BaseShaderRegister + itemInRange;
                bool found = false;
                D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle = m_resources.samplerHeap.GetCpuHandle(descriptorTableBaseIndex + range.OffsetInDescriptorsFromTableStart + itemInRange);

                for (const auto& binding : desc.bindings)
                {
                    if (binding.type == ResourceType::Sampler && binding.slot == slot)
                    {
                        Sampler* sampler = core::MemoryUtilities::CheckedCast<Sampler*>(binding.resourcePtr);
                        resources.push_back(sampler);

                        sampler->CreateDescriptor(descriptorHandle.ptr);
                        found = true;
                        break;
                    }
                }

                if (!found)
                {
                    D3D12_SAMPLER_DESC samplerDesc = {};
                    m_context.device->CreateSampler(&samplerDesc, descriptorHandle);
                }
            }
        }

        m_resources.samplerHeap.CopyToShaderVisibleHeap(descriptorTableBaseIndex, layout->descriptorTableSizeSamplers);
    }

    if (layout->descriptorTableSizeSrvEtc > 0)
    {
        DescriptorIndex descriptorTableBaseIndex = m_resources.shaderResourceViewHeap.AllocateDescriptors(layout->descriptorTableSizeSrvEtc);
        descriptorTableSrvEtc = descriptorTableBaseIndex;
        rootParameterIndexSrvEtc = layout->rootParameterSrvEtc;
        descriptorTableValidSrvEtc = true;

        for (const auto& range : layout->descriptorRangesSrvEtc)
        {
            for (uint32_t itemInRange = 0; itemInRange < range.NumDescriptors; itemInRange++)
            {
                uint32_t slot = range.BaseShaderRegister + itemInRange;
                bool found = false;
                D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle = m_resources.shaderResourceViewHeap.GetCpuHandle(descriptorTableBaseIndex + range.OffsetInDescriptorsFromTableStart + itemInRange);

                IResource* pResource = nullptr;

                for (size_t bindingIndex = 0; bindingIndex < desc.bindings.size(); bindingIndex++)
                {
                    const BindingSetItem& binding = desc.bindings[bindingIndex];

                    if (binding.slot != slot)
                    {
                        continue;
                    }

                    const auto bindingType = Utility::GetNormalizedResourceType(binding.type);

                    if (range.RangeType == D3D12_DESCRIPTOR_RANGE_TYPE_SRV && bindingType == ResourceType::TypedBuffer_SRV)
                    {
                        if (binding.resourcePtr)
                        {
                            Buffer* buffer = core::MemoryUtilities::CheckedCast<Buffer*>(binding.resourcePtr);
                            pResource = buffer;

                            buffer->CreateSRV(descriptorHandle.ptr, binding.format, binding.range, binding.type);

                            if (!buffer->permanentState.Any())
                            {
                                bindingsThatNeedTransitions.push_back(static_cast<uint16_t>(bindingIndex));
                            }
                            else
                            {
                                CommandListResourceStateTracker::VerifyPermanentResourceState(
                                    buffer->permanentState, ResourceStates::ShaderResource,
                                    false, buffer->desc.debugName);
                            }
                        } else
                        {
                            Buffer::CreateNullSRV(descriptorHandle.ptr, binding.format, m_context);
                        }

                        found = true;
                        break;
                    }
                    else if (range.RangeType == D3D12_DESCRIPTOR_RANGE_TYPE_UAV && bindingType == ResourceType::TypedBuffer_UAV)
                    {

                        if (binding.resourcePtr)
                        {
                            Buffer* buffer = core::MemoryUtilities::CheckedCast<Buffer*>(binding.resourcePtr);
                            pResource = buffer;

                            buffer->CreateUAV(descriptorHandle.ptr, binding.format, binding.range, binding.type);

                            if (!buffer->permanentState.Any())
                            {
                                bindingsThatNeedTransitions.push_back(static_cast<uint16_t>(bindingIndex));
                            }
                            else
                            {
                                CommandListResourceStateTracker::VerifyPermanentResourceState(
                                    buffer->permanentState, ResourceStates::UnorderedAccess,
                                    false, buffer->desc.debugName);
                            }
                        }
                        else
                        {
                            Buffer::CreateNullUAV(descriptorHandle.ptr, binding.format, m_context);
                        }

                        hasUavBindings = true;
                        found = true;
                        break;
                    }
                    else if (range.RangeType == D3D12_DESCRIPTOR_RANGE_TYPE_SRV && bindingType == ResourceType::Texture_SRV)
                    {
                        Texture* texture = core::MemoryUtilities::CheckedCast<Texture*>(binding.resourcePtr);

                        TextureSubresourceSet subresources = binding.subresources;

                        texture->CreateSRV(descriptorHandle.ptr, binding.format, binding.dimension, subresources);
                        pResource = texture;

                        if (!texture->permanentState.Any())
                        {
                            bindingsThatNeedTransitions.push_back(static_cast<uint16_t>(bindingIndex));
                        }
                        else
                        {
                            CommandListResourceStateTracker::VerifyPermanentResourceState(
                                texture->permanentState, ResourceStates::ShaderResource,
                                true, texture->desc.debugName);
                        }

                        found = true;
                        break;
                    }
                    else if (range.RangeType == D3D12_DESCRIPTOR_RANGE_TYPE_UAV && bindingType == ResourceType::Texture_UAV)
                    {
                        Texture* texture = core::MemoryUtilities::CheckedCast<Texture*>(binding.resourcePtr);

                        TextureSubresourceSet subresources = binding.subresources;

                        texture->CreateUAV(descriptorHandle.ptr, binding.format, binding.dimension, subresources);
                        pResource = texture;

                        if (!texture->permanentState.Any())
                        {
                            bindingsThatNeedTransitions.push_back(static_cast<uint16_t>(bindingIndex));
                        }
                        else
                        {
                            CommandListResourceStateTracker::VerifyPermanentResourceState(
                                texture->permanentState, ResourceStates::UnorderedAccess,
                                true, texture->desc.debugName);
                        }

                        hasUavBindings = true;
                        found = true;
                        break;
                    }
                    else if (range.RangeType == D3D12_DESCRIPTOR_RANGE_TYPE_SRV && bindingType == ResourceType::RayTracingAccelStruct)
                    {
                        AccelStruct* accelStruct = core::MemoryUtilities::CheckedCast<AccelStruct*>(binding.resourcePtr);
                        accelStruct->CreateSRV(descriptorHandle.ptr);
                        pResource = accelStruct;

                        bindingsThatNeedTransitions.push_back(static_cast<uint16_t>(bindingIndex));

                        found = true;
                        break;
                    }
                    else if (range.RangeType == D3D12_DESCRIPTOR_RANGE_TYPE_CBV && bindingType == ResourceType::ConstantBuffer)
                    {
                        Buffer* buffer = core::MemoryUtilities::CheckedCast<Buffer*>(binding.resourcePtr);

                        buffer->CreateCBV(descriptorHandle.ptr, binding.range);
                        pResource = buffer;

                        if (buffer->desc.volatileBuffer)
                        {
                            found = false;
                            break;
                        }
                        else
                        {
                            if (!buffer->permanentState.Any())
                            {
                                bindingsThatNeedTransitions.push_back(static_cast<uint16_t>(bindingIndex));
                            }
                            else
                            {
                                CommandListResourceStateTracker::VerifyPermanentResourceState(
                                    buffer->permanentState, ResourceStates::ConstantBuffer,
                                    false, buffer->desc.debugName);
                            }
                        }

                        found = true;
                        break;
                    }
                }

                if (pResource)
                {
                    resources.push_back(pResource);
                }

                if (!found)
                {
                    // Create a null SRV, UAV, or CBV

                    switch (range.RangeType)
                    {
                    case D3D12_DESCRIPTOR_RANGE_TYPE_SRV:
                        Buffer::CreateNullSRV(descriptorHandle.ptr, FormatType::UNKNOWN, m_context);
                        break;

                    case D3D12_DESCRIPTOR_RANGE_TYPE_UAV:
                        Buffer::CreateNullUAV(descriptorHandle.ptr, FormatType::UNKNOWN, m_context);
                        break;

                    case D3D12_DESCRIPTOR_RANGE_TYPE_CBV:
                        m_context.device->CreateConstantBufferView(nullptr, descriptorHandle);
                        break;

                    case D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER:
                    default:
                        SLUG_LOG(RHILogCategory, core::LogType::Error, "InvalidEnum RangeType");
                        break;
                    }
                }
            }
        }

        m_resources.shaderResourceViewHeap.CopyToShaderVisibleHeap(descriptorTableBaseIndex, layout->descriptorTableSizeSrvEtc);
    }
}

}
