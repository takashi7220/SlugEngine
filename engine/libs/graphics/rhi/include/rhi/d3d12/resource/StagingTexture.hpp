#pragma once

#include "rhi/interface/ITexture.hpp"
#include "rhi/common/StateTracking.hpp"
#include "rhi/d3d12/resource/DeviceResources.hpp"
#include "rhi/d3d12/resource/Buffer.hpp"

namespace slug::rhi::d3d12
{

class StagingTexture : public IStagingTexture
{
public:
    struct SliceRegion
    {
        uint32_t offset = 0;
        size_t size = 0;
        D3D12_PLACED_SUBRESOURCE_FOOTPRINT footprint {};
    };

public:
    SliceRegion GetSliceRegion(ID3D12Device* device, const TextureSlice& slice);
    size_t GetSizeInBytes(ID3D12Device* device);
    void ComputeSubresourceOffsets(ID3D12Device* device);
    const TextureDesc& GetDesc() const override;
    Object GetNativeObject(ObjectType objectType) override;

public:
    TextureDesc desc;
    D3D12_RESOURCE_DESC resourceDesc {};
    core::TReferencePtr<Buffer> buffer;
    CpuAccessMode cpuAccess = CpuAccessMode::None;
    core::TVector<uint64_t> subresourceOffsets;
    core::TReferencePtr<ID3D12Fence> lastUseFence;
    uint64_t lastUseFenceValue = 0;
    SliceRegion mappedRegion;
    CpuAccessMode mappedAccess = CpuAccessMode::None;
};

}
