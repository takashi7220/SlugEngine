#pragma once

#include "rhi/interface/ITexture.hpp"
#include "rhi/common/StateTracking.hpp"
#include "rhi/d3d12/resource/DeviceResources.hpp"

namespace slug
{
namespace rhi
{
namespace d3d12
{

struct TextureBindingKey : public TextureSubresourceSet
{
    FormatType format = FormatType::UNKNOWN;
    bool readOnlyDsv = true;

    TextureBindingKey()
    {}

    TextureBindingKey(const TextureSubresourceSet& b, FormatType _format, bool _readOnlyDsv = false)
        : TextureSubresourceSet(b)
        , format(_format)
        , readOnlyDsv(_readOnlyDsv)
    {}

    bool operator== (const TextureBindingKey& other) const
    {
        return format == other.format &&
            static_cast<const TextureSubresourceSet&>(*this) == static_cast<const TextureSubresourceSet&>(other) &&
            readOnlyDsv == other.readOnlyDsv;
    }
};

struct TextureBindingKeyHash
{
    size_t operator()(TextureBindingKey key) const
    {
        size_t h1 = core::THash<FormatType>()(key.format);
        size_t h2 = core::THash<bool>()(key.readOnlyDsv);
        return h1 ^ h2;
    }
};

template<typename T>
using TextureBindingKeyHashMap = core::TUnorderedMap<TextureBindingKey, T, TextureBindingKeyHash>;

class Texture : public ITexture, public TextureStateExtension
{
public:
    Texture(const Context& context, DeviceResources& resources, TextureDesc desc, const D3D12_RESOURCE_DESC& resourceDesc);
    ~Texture() override;

    const TextureDesc& GetDesc() const override;
    Object GetNativeObject(ObjectType objectType) override;
    Object GetNativeView(ObjectType objectType, FormatType format, TextureSubresourceSet subresources, TextureDimension dimension, bool isReadOnlyDSV = false) override;

    void PostCreate();
    void CreateSRV(size_t descriptor, FormatType format, TextureDimension dimension, TextureSubresourceSet subresources) const;
    void CreateUAV(size_t descriptor, FormatType format, TextureDimension dimension, TextureSubresourceSet subresources) const;
    void CreateRTV(size_t descriptor, FormatType format, TextureSubresourceSet subresources) const;
    void CreateDSV(size_t descriptor, TextureSubresourceSet subresources, bool isReadOnly = false) const;
    DescriptorIndex GetClearMipLevelUAV(uint32_t mipLevel);
public:
    const TextureDesc desc;
    const D3D12_RESOURCE_DESC resourceDesc;
    core::TReferencePtr<ID3D12Resource> resource;
    uint8_t planeCount = 1;
    HANDLE sharedHandle = nullptr;
    HeapPtr heap;

private:
    const Context& m_context;
    DeviceResources& m_resources;
    TextureBindingKeyHashMap<DescriptorIndex> m_renderTargetViews;
    TextureBindingKeyHashMap<DescriptorIndex> m_depthStencilViews;
    TextureBindingKeyHashMap<DescriptorIndex> m_customSrvs;
    TextureBindingKeyHashMap<DescriptorIndex> m_customUavs;
    core::TVector<DescriptorIndex> m_clearMipLevelUavs;
};
}
}
}
