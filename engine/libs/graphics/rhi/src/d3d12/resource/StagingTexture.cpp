#include "rhi/d3d12/D3D12Utility.hpp"
#include "rhi/d3d12/resource/StagingTexture.hpp"
#include "rhi/common/Resource.hpp"

namespace slug::rhi::d3d12
{

StagingTexture::SliceRegion StagingTexture::GetSliceRegion(ID3D12Device* device, const TextureSlice& slice)
{
    SliceRegion ret;
    const uint64_t subresource = D3D12Utility::CalcSubresource(slice.mipLevel, slice.arraySlice, 0, desc.mipLevels, desc.arraySize);

    SLUG_ASSERT(subresource < subresourceOffsets.size());

    uint64_t size = 0;
    device->GetCopyableFootprints(&resourceDesc, UINT(subresource), 1, subresourceOffsets[subresource], &ret.footprint, nullptr, nullptr, &size);
    ret.offset = off_t(ret.footprint.Offset);
    ret.size = size;
    return ret;
}

size_t StagingTexture::GetSizeInBytes(ID3D12Device* device)
{
    const uint64_t lastSubresource = D3D12Utility::CalcSubresource(desc.mipLevels - 1, desc.arraySize - 1, 0, desc.mipLevels, desc.arraySize);
    SLUG_ASSERT(lastSubresource < subresourceOffsets.size());

    uint64_t lastSubresourcesize_t = 0;
    device->GetCopyableFootprints(&resourceDesc, UINT(lastSubresource), 1, 0, nullptr, nullptr, nullptr, &lastSubresourcesize_t);

    return subresourceOffsets[lastSubresource] + lastSubresourcesize_t;

}

void StagingTexture::ComputeSubresourceOffsets(ID3D12Device* device)
{
    const uint32_t lastSubresource = D3D12Utility::CalcSubresource(desc.mipLevels - 1, desc.arraySize - 1, 0, desc.mipLevels, desc.arraySize);

    const uint32_t numSubresources = lastSubresource + 1;
    subresourceOffsets.resize(numSubresources);

    uint64_t baseOffset = 0;
    for (uint32_t i = 0; i < lastSubresource + 1; i++)
    {
        uint64_t subresourcesize_t;
        device->GetCopyableFootprints(&resourceDesc, i, 1, 0, nullptr, nullptr, nullptr, &subresourcesize_t);
        subresourceOffsets[i] = baseOffset;
        baseOffset += subresourcesize_t;
        baseOffset = D3D12_TEXTURE_DATA_PLACEMENT_ALIGNMENT * ((baseOffset + D3D12_TEXTURE_DATA_PLACEMENT_ALIGNMENT - 1) / D3D12_TEXTURE_DATA_PLACEMENT_ALIGNMENT);
    }
}

const TextureDesc& StagingTexture::GetDesc() const
{
    return desc;
}

Object StagingTexture::GetNativeObject(ObjectType objectType)
{
    switch (objectType)
    {
    case object_types::D3D12_Resource:
        return Object(buffer->resource);
    default:
        return nullptr;
    }
}

}
