#include "rhi/d3d12/resource/BindlessLayout.hpp"
#include "rhi/d3d12/D3D12Utility.hpp"

namespace slug::rhi::d3d12
{

BindlessLayout::BindlessLayout(const BindlessLayoutDesc& desc)
    : desc(desc)
{
    descriptorRanges.resize(0);

    for (const BindingLayoutItem& item : desc.registerSpaces)
    {
        D3D12_DESCRIPTOR_RANGE_TYPE rangeType;

        switch (item.type)
        {
        case ResourceType::Texture_SRV:
            [[fallthrough]];
        case ResourceType::TypedBuffer_SRV:
            [[fallthrough]];
        case ResourceType::StructuredBuffer_SRV:
            [[fallthrough]];
        case ResourceType::RawBuffer_SRV:
            [[fallthrough]];
        case ResourceType::RayTracingAccelStruct:
            rangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
            break;

        case ResourceType::ConstantBuffer:
            rangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
            break;

        case ResourceType::Texture_UAV:
            [[fallthrough]];
        case ResourceType::TypedBuffer_UAV:
            [[fallthrough]];
        case ResourceType::StructuredBuffer_UAV:
            [[fallthrough]];
        case ResourceType::RawBuffer_UAV:
            rangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
            break;

        case ResourceType::Sampler:
            rangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
            break;

        case ResourceType::None:
            [[fallthrough]];
        case ResourceType::VolatileConstantBuffer:
            [[fallthrough]];
        case ResourceType::PushConstants:
            [[fallthrough]];
        case ResourceType::Count:
            [[fallthrough]];
        default:
            SLUG_ASSERT_MSG(false, "Invlid Enum");
            continue;
        }

        D3D12_DESCRIPTOR_RANGE1& descriptorRange = descriptorRanges.emplace_back();

        descriptorRange.RangeType = rangeType;
        descriptorRange.NumDescriptors = ~0u;
        descriptorRange.BaseShaderRegister = desc.firstSlot;
        descriptorRange.RegisterSpace = item.slot;
        descriptorRange.Flags = D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE;
        descriptorRange.OffsetInDescriptorsFromTableStart = 0;
    }

    rootParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    rootParameter.ShaderVisibility = D3D12Utility::ConvertShaderStage(desc.visibility);
    rootParameter.DescriptorTable.NumDescriptorRanges = uint32_t(descriptorRanges.size());
    rootParameter.DescriptorTable.pDescriptorRanges = &descriptorRanges[0];
}
}
