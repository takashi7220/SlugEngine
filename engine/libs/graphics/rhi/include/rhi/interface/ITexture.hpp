#pragma once

#include "rhi/ResourceType.hpp"

namespace slug
{
namespace rhi
{
struct TextureDesc
{
    uint32_t width = 0;
    uint32_t height = 0;
    uint32_t depth = 1;
    uint32_t arraySize = 1;
    uint32_t mipLevels = 1;
    uint32_t sampleCount = 1;
    uint32_t sampleQuality = 0;
    FormatType format = FormatType::UNKNOWN;
    TextureDimension dimension = TextureDimension::Texture2D;
    core::String debugName = "";
    bool renderTarget = false;
    bool uav = false;
    bool typeless = false;
    bool stagingRateSurface = false;
    SharedResourceFlagsBitset sharedResourceFlags = SharedResourceFlags::None;
    bool virtualTexture = false;
    Color clearColor = {};
    float clearDepth = 0.0f;
    uint8_t clearStencil = 0x00;
    bool useClearValue = false;
    ResourceStatesBitset initialState = {};
    bool keepInitialState = false;
};

struct TextureSlice
{
    uint32_t x = 0;
    uint32_t y = 0;
    uint32_t z = 0;
    uint32_t width = UINT_MAX;
    uint32_t height = UINT_MAX;
    uint32_t depth = UINT_MAX;
    MipLevel mipLevel = 0;
    ArraySlice arraySlice = 0;

    TextureSlice Resolve(const TextureDesc& desc) const
    {
        TextureSlice ret(*this);

        SLUG_ASSERT(mipLevel < desc.mipLevels);

        if (width == uint32_t(-1))
        {
            ret.width = (desc.width >> mipLevel);
        }

        if (height == uint32_t(-1))
        {
            ret.height = (desc.height >> mipLevel);
        }

        if (depth == uint32_t(-1)) {
            if (desc.dimension == TextureDimension::Texture3D)
            {
                ret.depth = (desc.depth >> mipLevel);
            }
            else
            {
                ret.depth = 1;
            }
        }
        return ret;
    }
};

struct TextureSubresourceSet
{
    static constexpr MipLevel AllMipLevels = INT_MAX;
    static constexpr MipLevel AllArraySlices = INT_MAX;

    MipLevel baseMipLevel = 0;
    MipLevel numMipLevels = 1;
    ArraySlice baseArraySlice = 0;
    ArraySlice numArraySlices = 1;

    struct Hash
    {
        size_t operator()(const TextureSubresourceSet& subresouce) const
        {
            size_t hash = 0;
            rhi::HashCombine(hash, subresouce.baseMipLevel);
            rhi::HashCombine(hash, subresouce.numMipLevels);
            rhi::HashCombine(hash, subresouce.baseArraySlice);
            rhi::HashCombine(hash, subresouce.numArraySlices);
            return hash;
        }
    };

    TextureSubresourceSet() = default;

    TextureSubresourceSet(MipLevel _baseMipLevel, MipLevel _numMipLevels, ArraySlice _baseArraySlice, ArraySlice _numArraySlices)
        : baseMipLevel(_baseMipLevel)
        , numMipLevels(_numMipLevels)
        , baseArraySlice(_baseArraySlice)
        , numArraySlices(_numArraySlices)
    {}

    TextureSubresourceSet Resolve(const TextureDesc& desc, bool single_mip_level) const
    {
        TextureSubresourceSet ret;
        ret.baseMipLevel = baseMipLevel;

        if (single_mip_level) {
            ret.numMipLevels = 1;
        }
        else {
            uint32_t last_mip_level_plus_one = math::TMin(baseMipLevel + numMipLevels, desc.mipLevels);
            ret.numMipLevels = MipLevel(math::TMax(0u, last_mip_level_plus_one - baseMipLevel));
        }

        switch (desc.dimension) {
        case TextureDimension::Texture1DArray:
        case TextureDimension::Texture2DArray:
        case TextureDimension::TextureCube:
        case TextureDimension::TextureCubeArray:
        case TextureDimension::Texture2DMSArray:
        {
            ret.baseArraySlice = baseArraySlice;
            uint32_t last_array_slice_plus_one = math::TMin(baseArraySlice + numArraySlices, desc.arraySize);
            ret.numArraySlices = ArraySlice(math::TMax(0u, last_array_slice_plus_one - baseArraySlice));
            break;
        }
        default:
            ret.baseArraySlice = 0;
            ret.numArraySlices = 1;
            break;
        }

        return ret;
    }

    bool CheckEntireTexture(const TextureDesc& desc) const
    {
        if (baseMipLevel > 0u || baseMipLevel + numMipLevels < desc.mipLevels)
            return false;

        switch (desc.dimension) {
        case TextureDimension::Texture1DArray:
            [[fallthrough]];
        case TextureDimension::Texture2DArray:
            [[fallthrough]];
        case TextureDimension::TextureCube:
            [[fallthrough]];
        case TextureDimension::TextureCubeArray:
            [[fallthrough]];
        case TextureDimension::Texture2DMSArray:
            if (baseArraySlice > 0u || baseArraySlice + numArraySlices < desc.arraySize) {
                return false;
            }
            break;
        default:
            return true;
        }
        return false;
    }

    bool operator ==(const TextureSubresourceSet& other) const
    {
        return baseMipLevel == other.baseMipLevel &&
            numMipLevels == other.numMipLevels &&
            baseArraySlice == other.baseArraySlice &&
            numArraySlices == other.numArraySlices;
    }
    bool operator !=(const TextureSubresourceSet& other) const
    {
        return !(*this == other);
    }
};
static const TextureSubresourceSet AllSubresources = TextureSubresourceSet(0, TextureSubresourceSet::AllMipLevels, 0, TextureSubresourceSet::AllArraySlices);


class ITexture : public IResource
{
public:
    virtual const TextureDesc& GetDesc() const = 0;

    virtual Object GetNativeView(
        ObjectType objectType,
        FormatType format = FormatType::UNKNOWN,
        TextureSubresourceSet subresources = AllSubresources,
        TextureDimension dimension = TextureDimension::Unknown,
        bool readOnlyDsv = false) = 0;
};

using TexturePtr = core::TReferencePtr<ITexture>;

class IStagingTexture : public IResource
{
public:
    virtual const TextureDesc& GetDesc() const = 0;
};

using StagingTexturePtr = core::TReferencePtr<IStagingTexture>;

}
}
