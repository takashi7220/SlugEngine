
#include "rhi/d3d12/D3D12Utility.hpp"
#include "rhi/common/Resource.hpp"
#include "rhi/common/Utility.hpp"
#include "rhi/d3d12/resource/Context.hpp"
#include "rhi/d3d12/resource/Texture.hpp"

namespace slug
{
namespace rhi
{
namespace d3d12
{
Texture::Texture(const Context& context, DeviceResources& resources, TextureDesc desc, const D3D12_RESOURCE_DESC& resourceDesc)
    : TextureStateExtension(this->desc)
    , desc(std::move(desc))
    , resourceDesc(resourceDesc)
    , m_context(context)
    , m_resources(resources)
{
    TextureStateExtension::stateInitialized = true;
}

Texture::~Texture()
{
    for (auto& pair : m_renderTargetViews)
    {
        m_resources.renderTargetViewHeap.ReleaseDescriptor(pair.second);
    }

    for (auto& pair : m_depthStencilViews)
    {
        m_resources.depthStencilViewHeap.ReleaseDescriptor(pair.second);
    }

    for (auto& index : m_clearMipLevelUavs)
    {
        m_resources.shaderResourceViewHeap.ReleaseDescriptor(index);
    }

    for (auto& pair : m_customSrvs)
    {
        m_resources.shaderResourceViewHeap.ReleaseDescriptor(pair.second);
    }

    for (auto& pair : m_customUavs)
    {
        m_resources.shaderResourceViewHeap.ReleaseDescriptor(pair.second);
    }
}

const TextureDesc& Texture::GetDesc() const
{
    return desc;
}

Object Texture::GetNativeObject(ObjectType objectType)
{
    switch (objectType)
    {
    case object_types::D3D12_Resource:
        return Object(resource);
    case object_types::SharedHandle:
        return Object(sharedHandle);
    default:
        return nullptr;
    }
}

Object Texture::GetNativeView(ObjectType objectType, FormatType format, TextureSubresourceSet subresources, TextureDimension dimension, bool isReadOnlyDSV)
{
    SLUG_STATIC_ASSERT_MSG(sizeof(void*) == sizeof(D3D12_CPU_DESCRIPTOR_HANDLE), "Cannot typecast a descriptor to void*");

    switch (objectType)
    {
    case rhi::object_types::D3D12_ShaderResourceViewGpuDescripror:
        {
            TextureBindingKey key = TextureBindingKey(subresources, format);
            DescriptorIndex descriptorIndex;
            auto found = m_customSrvs.find(key);
            if (found == m_customSrvs.end())
            {
                descriptorIndex = m_resources.shaderResourceViewHeap.AllocateDescriptor();
                m_customSrvs[key] = descriptorIndex;

                const D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = m_resources.shaderResourceViewHeap.GetCpuHandle(descriptorIndex);
                CreateSRV(cpuHandle.ptr, format, dimension, subresources);
                m_resources.shaderResourceViewHeap.CopyToShaderVisibleHeap(descriptorIndex);
            }
            else
            {
                descriptorIndex = found->second;
            }

            return Object(m_resources.shaderResourceViewHeap.GetGpuHandle(descriptorIndex).ptr);
        }

    case rhi::object_types::D3D12_UnorderedAccessViewGpuDescripror:
        {
            TextureBindingKey key = TextureBindingKey(subresources, format);
            DescriptorIndex descriptorIndex;
            auto found = m_customUavs.find(key);
            if (found == m_customUavs.end())
            {
                descriptorIndex = m_resources.shaderResourceViewHeap.AllocateDescriptor();
                m_customUavs[key] = descriptorIndex;

                const D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = m_resources.shaderResourceViewHeap.GetCpuHandle(descriptorIndex);
                CreateUAV(cpuHandle.ptr, format, dimension, subresources);
                m_resources.shaderResourceViewHeap.CopyToShaderVisibleHeap(descriptorIndex);
            }
            else
            {
                descriptorIndex = found->second;
            }

            return Object(m_resources.shaderResourceViewHeap.GetGpuHandle(descriptorIndex).ptr);
        }
    case rhi::object_types::D3D12_RenderTargetViewDescriptor:
        {
            TextureBindingKey key = TextureBindingKey(subresources, format);
            DescriptorIndex descriptorIndex;

            auto found = m_renderTargetViews.find(key);
            if (found == m_renderTargetViews.end())
            {
                descriptorIndex = m_resources.renderTargetViewHeap.AllocateDescriptor();
                m_renderTargetViews[key] = descriptorIndex;

                const D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = m_resources.renderTargetViewHeap.GetCpuHandle(descriptorIndex);
                CreateRTV(cpuHandle.ptr, format, subresources);
            } else
            {
                descriptorIndex = found->second;
            }

            return Object(m_resources.renderTargetViewHeap.GetCpuHandle(descriptorIndex).ptr);
        }

    case rhi::object_types::D3D12_DepthStencilViewDescriptor:
        {
            TextureBindingKey key = TextureBindingKey(subresources, format, isReadOnlyDSV);
            DescriptorIndex descriptorIndex;

            auto found = m_depthStencilViews.find(key);
            if (found == m_depthStencilViews.end())
            {
                descriptorIndex = m_resources.depthStencilViewHeap.AllocateDescriptor();
                m_depthStencilViews[key] = descriptorIndex;

                const D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = m_resources.depthStencilViewHeap.GetCpuHandle(descriptorIndex);
                CreateDSV(cpuHandle.ptr, subresources, isReadOnlyDSV);
            }
            else
            {
                descriptorIndex = found->second;
            }

            return Object(m_resources.depthStencilViewHeap.GetCpuHandle(descriptorIndex).ptr);
        }

    default:
        return nullptr;
    }
}

void Texture::PostCreate()
{
    if (!desc.debugName.empty())
    {
        resource->SetName(core::StringUtility::GetWstring(desc.debugName.c_str()).c_str());
    }

    if (desc.uav)
    {
        m_clearMipLevelUavs.resize(desc.mipLevels);
        std::fill(m_clearMipLevelUavs.begin(), m_clearMipLevelUavs.end(), INVALID_DESCRIPTOR_INDEX);
    }

    planeCount = uint8_t(m_resources.GetFormatPlaneCount(resourceDesc.Format));
}

void Texture::CreateSRV(size_t descriptor, FormatType format, TextureDimension dimension, TextureSubresourceSet subresources) const
{
    subresources = subresources.Resolve(desc, false);

    if (dimension == TextureDimension::Unknown)
    {
        dimension = desc.dimension;
    }

    D3D12_SHADER_RESOURCE_VIEW_DESC viewDesc = {};

    viewDesc.Format = D3D12Utility::GetDxgiFormatMapping(format == FormatType::UNKNOWN ? desc.format : format).srvFormat;
    viewDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

    uint32_t planeSlice = (viewDesc.Format == DXGI_FORMAT_X24_TYPELESS_G8_UINT) ? 1 : 0;

    switch (dimension)
    {
    case TextureDimension::Texture1D:
        viewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE1D;
        viewDesc.Texture1D.MostDetailedMip = subresources.baseMipLevel;
        viewDesc.Texture1D.MipLevels = subresources.numMipLevels;
        break;
    case TextureDimension::Texture1DArray:
        viewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE1DARRAY;
        viewDesc.Texture1DArray.FirstArraySlice = subresources.baseArraySlice;
        viewDesc.Texture1DArray.ArraySize = subresources.numArraySlices;
        viewDesc.Texture1DArray.MostDetailedMip = subresources.baseMipLevel;
        viewDesc.Texture1DArray.MipLevels = subresources.numMipLevels;
        break;
    case TextureDimension::Texture2D:
        viewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        viewDesc.Texture2D.MostDetailedMip = subresources.baseMipLevel;
        viewDesc.Texture2D.MipLevels = subresources.numMipLevels;
        viewDesc.Texture2D.PlaneSlice = planeSlice;
        break;
    case TextureDimension::Texture2DArray:
        viewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
        viewDesc.Texture2DArray.FirstArraySlice = subresources.baseArraySlice;
        viewDesc.Texture2DArray.ArraySize = subresources.numArraySlices;
        viewDesc.Texture2DArray.MostDetailedMip = subresources.baseMipLevel;
        viewDesc.Texture2DArray.MipLevels = subresources.numMipLevels;
        viewDesc.Texture2DArray.PlaneSlice = planeSlice;
        break;
    case TextureDimension::TextureCube:
        viewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
        viewDesc.TextureCube.MostDetailedMip = subresources.baseMipLevel;
        viewDesc.TextureCube.MipLevels = subresources.numMipLevels;
        break;
    case TextureDimension::TextureCubeArray:
        viewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBEARRAY;
        viewDesc.TextureCubeArray.First2DArrayFace = subresources.baseArraySlice;
        viewDesc.TextureCubeArray.NumCubes = subresources.numArraySlices / 6;
        viewDesc.TextureCubeArray.MostDetailedMip = subresources.baseMipLevel;
        viewDesc.TextureCubeArray.MipLevels = subresources.numMipLevels;
        break;
    case TextureDimension::Texture2DMS:
        viewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DMS;
        break;
    case TextureDimension::Texture2DMSArray:
        viewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DMSARRAY;
        viewDesc.Texture2DMSArray.FirstArraySlice = subresources.baseArraySlice;
        viewDesc.Texture2DMSArray.ArraySize = subresources.numArraySlices;
        break;
    case TextureDimension::Texture3D:
        viewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE3D;
        viewDesc.Texture3D.MostDetailedMip = subresources.baseMipLevel;
        viewDesc.Texture3D.MipLevels = subresources.numMipLevels;
        break;
    case TextureDimension::Unknown:
        [[fallthrough]];
    default:
        SLUG_LOG(RHILogCategory, core::LogType::Error, "InvalidEnum TextureDimension");
        return;
    }

    m_context.device->CreateShaderResourceView(resource, &viewDesc, { descriptor });
}

void Texture::CreateUAV(size_t descriptor, FormatType format, TextureDimension dimension, TextureSubresourceSet subresources) const
{
    subresources = subresources.Resolve(desc, true);

    if (dimension == TextureDimension::Unknown)
    {
        dimension = desc.dimension;
    }

    D3D12_UNORDERED_ACCESS_VIEW_DESC viewDesc = {};

    viewDesc.Format = D3D12Utility::GetDxgiFormatMapping(format == FormatType::UNKNOWN ? desc.format : format).srvFormat;

    switch (desc.dimension)
    {
    case TextureDimension::Texture1D:
        viewDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE1D;
        viewDesc.Texture1D.MipSlice = subresources.baseMipLevel;
        break;
    case TextureDimension::Texture1DArray:
        viewDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE1DARRAY;
        viewDesc.Texture1DArray.FirstArraySlice = subresources.baseArraySlice;
        viewDesc.Texture1DArray.ArraySize = subresources.numArraySlices;
        viewDesc.Texture1DArray.MipSlice = subresources.baseMipLevel;
        break;
    case TextureDimension::Texture2D:
        viewDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
        viewDesc.Texture2D.MipSlice = subresources.baseMipLevel;
        break;
    case TextureDimension::Texture2DArray:
        [[fallthrough]];
    case TextureDimension::TextureCube:
        [[fallthrough]];
    case TextureDimension::TextureCubeArray:
        viewDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2DARRAY;
        viewDesc.Texture2DArray.FirstArraySlice = subresources.baseArraySlice;
        viewDesc.Texture2DArray.ArraySize = subresources.numArraySlices;
        viewDesc.Texture2DArray.MipSlice = subresources.baseMipLevel;
        break;
    case TextureDimension::Texture3D:
        viewDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE3D;
        viewDesc.Texture3D.FirstWSlice = 0;
        viewDesc.Texture3D.WSize = desc.depth;
        viewDesc.Texture3D.MipSlice = subresources.baseMipLevel;
        break;
    case TextureDimension::Texture2DMS:
        [[fallthrough]];
    case TextureDimension::Texture2DMSArray:
        {
            core::String txt = {};
            core::StringUtility::Sprintfs(txt, "Error : Texture [%s] has unsupported dimension for UAV, %s", desc.debugName.c_str(), Utility::TextureDimensionToString(desc.dimension));
            SLUG_ASSERT_MSG(false, txt.c_str());
            return;
        }
    case TextureDimension::Unknown:
        [[fallthrough]];
    default:
        SLUG_LOG(RHILogCategory, core::LogType::Error, "InvalidEnum TextureDimension");
        return;
    }

    m_context.device->CreateUnorderedAccessView(resource, nullptr, &viewDesc, { descriptor });
}

void Texture::CreateRTV(size_t descriptor, FormatType format, TextureSubresourceSet subresources) const
{
    subresources = subresources.Resolve(desc, true);

    D3D12_RENDER_TARGET_VIEW_DESC viewDesc = {};

    viewDesc.Format = D3D12Utility::GetDxgiFormatMapping(format == FormatType::UNKNOWN ? desc.format : format).rtvFormat;

    switch (desc.dimension)
    {
    case TextureDimension::Texture1D:
        viewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE1D;
        viewDesc.Texture1D.MipSlice = subresources.baseMipLevel;
        break;
    case TextureDimension::Texture1DArray:
        viewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE1DARRAY;
        viewDesc.Texture1DArray.FirstArraySlice = subresources.baseArraySlice;
        viewDesc.Texture1DArray.ArraySize = subresources.numArraySlices;
        viewDesc.Texture1DArray.MipSlice = subresources.baseMipLevel;
        break;
    case TextureDimension::Texture2D:
        viewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
        viewDesc.Texture2D.MipSlice = subresources.baseMipLevel;
        break;
    case TextureDimension::Texture2DArray:
        [[fallthrough]];
    case TextureDimension::TextureCube:
        [[fallthrough]];
    case TextureDimension::TextureCubeArray:
        viewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DARRAY;
        viewDesc.Texture2DArray.ArraySize = subresources.numArraySlices;
        viewDesc.Texture2DArray.FirstArraySlice = subresources.baseArraySlice;
        viewDesc.Texture2DArray.MipSlice = subresources.baseMipLevel;
        break;
    case TextureDimension::Texture2DMS:
        viewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DMS;
        break;
    case TextureDimension::Texture2DMSArray:
        viewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DMSARRAY;
        viewDesc.Texture2DMSArray.FirstArraySlice = subresources.baseArraySlice;
        viewDesc.Texture2DMSArray.ArraySize = subresources.numArraySlices;
        break;
    case TextureDimension::Texture3D:
        viewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE3D;
        viewDesc.Texture3D.FirstWSlice = subresources.baseArraySlice;
        viewDesc.Texture3D.WSize = subresources.numArraySlices;
        viewDesc.Texture3D.MipSlice = subresources.baseMipLevel;
        break;
    case TextureDimension::Unknown:
        [[fallthrough]];
    default:
        SLUG_LOG(RHILogCategory, core::LogType::Error, "InvalidEnum TextureDimension");
        return;
    }

    m_context.device->CreateRenderTargetView(resource, &viewDesc, { descriptor });
}

void Texture::CreateDSV(size_t descriptor, TextureSubresourceSet subresources, bool isReadOnly) const
{
    subresources = subresources.Resolve(desc, true);

    D3D12_DEPTH_STENCIL_VIEW_DESC viewDesc = {};

    viewDesc.Format = D3D12Utility::GetDxgiFormatMapping(desc.format).rtvFormat;

    if (isReadOnly)
    {
        viewDesc.Flags |= D3D12_DSV_FLAG_READ_ONLY_DEPTH;
        if (viewDesc.Format == DXGI_FORMAT_D24_UNORM_S8_UINT || viewDesc.Format == DXGI_FORMAT_D32_FLOAT_S8X24_UINT)
            viewDesc.Flags |= D3D12_DSV_FLAG_READ_ONLY_STENCIL;
    }

    switch (desc.dimension)
    {
    case TextureDimension::Texture1D:
        viewDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE1D;
        viewDesc.Texture1D.MipSlice = subresources.baseMipLevel;
        break;
    case TextureDimension::Texture1DArray:
        viewDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE1DARRAY;
        viewDesc.Texture1DArray.FirstArraySlice = subresources.baseArraySlice;
        viewDesc.Texture1DArray.ArraySize = subresources.numArraySlices;
        viewDesc.Texture1DArray.MipSlice = subresources.baseMipLevel;
        break;
    case TextureDimension::Texture2D:
        viewDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
        viewDesc.Texture2D.MipSlice = subresources.baseMipLevel;
        break;
    case TextureDimension::Texture2DArray:
        [[fallthrough]];
    case TextureDimension::TextureCube:
        [[fallthrough]];
    case TextureDimension::TextureCubeArray:
        viewDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DARRAY;
        viewDesc.Texture2DArray.ArraySize = subresources.numArraySlices;
        viewDesc.Texture2DArray.FirstArraySlice = subresources.baseArraySlice;
        viewDesc.Texture2DArray.MipSlice = subresources.baseMipLevel;
        break;
    case TextureDimension::Texture2DMS:
        viewDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DMS;
        break;
    case TextureDimension::Texture2DMSArray:
        viewDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DMSARRAY;
        viewDesc.Texture2DMSArray.FirstArraySlice = subresources.baseArraySlice;
        viewDesc.Texture2DMSArray.ArraySize = subresources.numArraySlices;
        break;
    case TextureDimension::Texture3D:
        {
            core::String txt = {};
            core::StringUtility::Sprintfs(txt, "Error : Texture [%s] has unsupported dimension for DSV, %s", desc.debugName.c_str(), Utility::TextureDimensionToString(desc.dimension));
            SLUG_ASSERT_MSG(false, txt.c_str());
            return;
        }
    case TextureDimension::Unknown:
    default:
        SLUG_LOG(RHILogCategory, core::LogType::Error, "InvalidEnum TextureDimension");
        return;
    }

    m_context.device->CreateDepthStencilView(resource, &viewDesc, { descriptor });
}

DescriptorIndex Texture::GetClearMipLevelUAV(uint32_t mipLevel)
{
    SLUG_ASSERT(desc.uav);

    DescriptorIndex descriptorIndex = m_clearMipLevelUavs[mipLevel];

    if (descriptorIndex != INVALID_DESCRIPTOR_INDEX)
    {
        return descriptorIndex;
    }

    descriptorIndex = m_resources.shaderResourceViewHeap.AllocateDescriptor();
    TextureSubresourceSet subresources(mipLevel, 1, 0, TextureSubresourceSet::AllArraySlices);
    CreateUAV(m_resources.shaderResourceViewHeap.GetCpuHandle(descriptorIndex).ptr, FormatType::UNKNOWN, TextureDimension::Unknown, subresources);
    m_resources.shaderResourceViewHeap.CopyToShaderVisibleHeap(descriptorIndex);
    m_clearMipLevelUavs[mipLevel] = descriptorIndex;

    return descriptorIndex;
}

}
}
}
