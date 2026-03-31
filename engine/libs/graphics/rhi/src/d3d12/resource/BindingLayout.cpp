#pragma once

#include "rhi/d3d12/resource/BindingLayout.hpp"
#include "rhi/common/Utility.hpp"
#include "rhi/d3d12/D3D12Utility.hpp"

namespace slug
{
namespace rhi
{
namespace d3d12
{
BindingLayout::BindingLayout(const BindingLayoutDesc& _desc)
    : desc(_desc)
{
    ResourceType currentType = ResourceType(-1);
    uint32_t currentSlot = ~0u;

    D3D12_ROOT_CONSTANTS rootConstants = {};

    for (const BindingLayoutItem& binding : desc.bindings) 
    {
        if (binding.type == ResourceType::VolatileConstantBuffer) 
        {
            D3D12_ROOT_DESCRIPTOR1 rootDescriptor;
            rootDescriptor.ShaderRegister = binding.slot;
            rootDescriptor.RegisterSpace = desc.registerSpace;
            rootDescriptor.Flags = D3D12_ROOT_DESCRIPTOR_FLAG_DATA_STATIC;

            rootParametersVolatileCb.push_back(core::MakePair(-1, rootDescriptor));
        }
        else if (binding.type == ResourceType::PushConstants) 
        {
            pushConstantByteSize = binding.size;
            rootConstants.ShaderRegister = binding.slot;
            rootConstants.RegisterSpace = desc.registerSpace;
            rootConstants.Num32BitValues = binding.size / 4;
        }
        else if (!Utility::AreResourceTypesCompatible(binding.type, currentType) || binding.slot != currentSlot + 1) 
        {

            if (binding.type == ResourceType::Sampler) 
            {
                descriptorRangesSamplers.resize(descriptorRangesSamplers.size() + 1);
                D3D12_DESCRIPTOR_RANGE1& range = descriptorRangesSamplers[descriptorRangesSamplers.size() - 1];

                range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
                range.NumDescriptors = 1;
                range.BaseShaderRegister = binding.slot;
                range.RegisterSpace = desc.registerSpace;
                range.OffsetInDescriptorsFromTableStart = descriptorTableSizeSamplers;
                range.Flags = D3D12_DESCRIPTOR_RANGE_FLAG_NONE;

                descriptorTableSizeSamplers += 1;
            }
            else {
                descriptorRangesSrvEtc.resize(descriptorRangesSrvEtc.size() + 1);
                D3D12_DESCRIPTOR_RANGE1& range = descriptorRangesSrvEtc[descriptorRangesSrvEtc.size() - 1];

                switch (binding.type) {
                case ResourceType::Texture_SRV:
                    [[fallthrough]];
                case ResourceType::TypedBuffer_SRV:
                    [[fallthrough]];
                case ResourceType::StructuredBuffer_SRV:
                    [[fallthrough]];
                case ResourceType::RawBuffer_SRV:
                    [[fallthrough]];
                case ResourceType::RayTracingAccelStruct:
                    range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
                    break;
                case ResourceType::Texture_UAV:
                    [[fallthrough]];
                case ResourceType::TypedBuffer_UAV:
                    [[fallthrough]];
                case ResourceType::StructuredBuffer_UAV:
                    [[fallthrough]];
                case ResourceType::RawBuffer_UAV:
                    range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
                    break;

                case ResourceType::ConstantBuffer:
                    range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
                    break;

                case ResourceType::None:
                    [[fallthrough]];
                case ResourceType::VolatileConstantBuffer:
                    [[fallthrough]];
                case ResourceType::Sampler:
                    [[fallthrough]];
                case ResourceType::PushConstants:
                    [[fallthrough]];
                case ResourceType::Count:
                    [[fallthrough]];
                default:
                    SLUG_LOG(RHILogCategory, core::LogType::Error, "InvalidEnum ResourceType");
                    continue;
                }
                range.NumDescriptors = 1;
                range.BaseShaderRegister = binding.slot;
                range.RegisterSpace = desc.registerSpace;
                range.OffsetInDescriptorsFromTableStart = descriptorTableSizeSrvEtc;
                range.Flags = D3D12_DESCRIPTOR_RANGE_FLAG_DATA_VOLATILE;

                descriptorTableSizeSrvEtc += 1;

                bindingLayoutsSrvEtc.push_back(binding);
            }

            currentType = binding.type;
            currentSlot = binding.slot;
        }
        else {
            // Extend the current range

            if (binding.type == ResourceType::Sampler) 
            {
                SLUG_ASSERT(!descriptorRangesSamplers.empty());
                D3D12_DESCRIPTOR_RANGE1& range = descriptorRangesSamplers[descriptorRangesSamplers.size() - 1];

                range.NumDescriptors += 1;
                descriptorTableSizeSamplers += 1;
            }
            else 
            {
                SLUG_ASSERT(!descriptorRangesSrvEtc.empty());
                D3D12_DESCRIPTOR_RANGE1& range = descriptorRangesSrvEtc[descriptorRangesSrvEtc.size() - 1];

                range.NumDescriptors += 1;
                descriptorTableSizeSrvEtc += 1;

                bindingLayoutsSrvEtc.push_back(binding);
            }

            currentSlot = binding.slot;
        }
    }

    rootParameters.resize(0);

    if (rootConstants.Num32BitValues) 
    {
        D3D12_ROOT_PARAMETER1& param = rootParameters.emplace_back();

        param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
        param.Constants = rootConstants;

        rootParameterPushConstants = RootParameterIndex(rootParameters.size() - 1);
    }

    for (core::TPair<RootParameterIndex, D3D12_ROOT_DESCRIPTOR1>& rootParameterVolatileCB : rootParametersVolatileCb) 
    {
        rootParameters.resize(rootParameters.size() + 1);
        D3D12_ROOT_PARAMETER1& param = rootParameters[rootParameters.size() - 1];

        param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
        param.ShaderVisibility = D3D12Utility::ConvertShaderStage(desc.visibility);
        param.Descriptor = rootParameterVolatileCB.second;

        rootParameterVolatileCB.first = RootParameterIndex(rootParameters.size() - 1);
    }

    if (descriptorTableSizeSamplers > 0) 
    {
        rootParameters.resize(rootParameters.size() + 1);
        D3D12_ROOT_PARAMETER1& param = rootParameters[rootParameters.size() - 1];

        param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
        param.ShaderVisibility = D3D12Utility::ConvertShaderStage(desc.visibility);
        param.DescriptorTable.NumDescriptorRanges = uint32_t(descriptorRangesSamplers.size());
        param.DescriptorTable.pDescriptorRanges = &descriptorRangesSamplers[0];

        rootParameterSamplers = RootParameterIndex(rootParameters.size() - 1);
    }

    if (descriptorTableSizeSrvEtc > 0) 
    {
        rootParameters.resize(rootParameters.size() + 1);
        D3D12_ROOT_PARAMETER1& param = rootParameters[rootParameters.size() - 1];

        param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
        param.ShaderVisibility = D3D12Utility::ConvertShaderStage(desc.visibility);
        param.DescriptorTable.NumDescriptorRanges = uint32_t(descriptorRangesSrvEtc.size());
        param.DescriptorTable.pDescriptorRanges = &descriptorRangesSrvEtc[0];

        rootParameterSrvEtc = RootParameterIndex(rootParameters.size() - 1);
    }
}
}
}
}
