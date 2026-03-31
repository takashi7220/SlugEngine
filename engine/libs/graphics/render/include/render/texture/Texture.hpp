#pragma once

#include "core/utility/Hash.hpp"

#include "rhi/Rhi.hpp"
#include "render/ResourceType.hpp"

namespace slug
{
namespace render
{

struct TextureSubresourceInfo
{
    size_t rowPitch = 0;
    size_t depthPitch = 0;
    ptrdiff_t dataOffset = 0;
    size_t dataSize = 0;

    bool operator==(const TextureSubresourceInfo& v) const
    {
        bool same = true;
        same &= rowPitch == v.rowPitch;
        same &= depthPitch == v.depthPitch;
        same &= dataOffset == v.dataOffset;
        same &= dataSize == v.dataSize;
        return same;
    }
};

struct ArraySubresourceInfo
{
    core::TVector<TextureSubresourceInfo> mipmapSubresources = {};

    bool operator==(ArraySubresourceInfo& v) const
    {
        bool same = true;
        same &= this->mipmapSubresources.size() == v.mipmapSubresources.size();
        if (same)
        {
            for (size_t i = 0; i < this->mipmapSubresources.size(); i++)
            {
                same &= this->mipmapSubresources.at(i) == v.mipmapSubresources.at(i);
            }
        }
        return same;
    }
};

struct TextureInfo
{
    uint32_t width = 0;
    uint32_t height = 0;
    uint32_t depth = 0;
    uint32_t arraySize = 0;
    uint32_t mipLevels = 0;
    render::TextureDimension dimension = render::TextureDimension::Texture2D;
    render::FormatType format = render::FormatType::UNKNOWN;
    bool useRenderTarget = false;
    char name[256];
    core::TVector<ArraySubresourceInfo> arraySubresources = {};

    bool operator==(const TextureInfo& v)
    {
        bool same = true;
        same &= width == v.width;
        same &= depth == v.height;
        same &= arraySize == v.arraySize;
        same &= mipLevels == v.mipLevels;
        same &= dimension == v.dimension;
        same &= format == v.format;
        same &= arraySubresources.size() == v.arraySubresources.size();
        if (same)
        {
            for (size_t i = 0; i < arraySubresources.size(); i++)
            {
                same &= arraySubresources.at(i) == v.arraySubresources.at(i);
            }
        }
        return same;
    }
};

struct SamplerInfo
{
    Color borderColor = { 1.0f, 1.0f, 1.0f, 1.0f };
    float maxAnisotropy = 1.f;
    float mipBias = 0.f;
    bool minFilter = true;
    bool magFilter = true;
    bool mipFilter = true;
    SamplerAddressMode addressU = SamplerAddressMode::Clamp;
    SamplerAddressMode addressV = SamplerAddressMode::Clamp;
    SamplerAddressMode addressW = SamplerAddressMode::Clamp;
    SamplerReductionType reductionType = SamplerReductionType::Standard;

    uint64_t GetHash() const
    {
        std::size_t seed = 0;
        core::HashUtility::HashCombine(seed, core::THash<float>{}(borderColor.r));
        core::HashUtility::HashCombine(seed, core::THash<float>{}(borderColor.g));
        core::HashUtility::HashCombine(seed, core::THash<float>{}(borderColor.b));
        core::HashUtility::HashCombine(seed, core::THash<float>{}(borderColor.a));
        core::HashUtility::HashCombine(seed, core::THash<float>{}(maxAnisotropy));
        core::HashUtility::HashCombine(seed, core::THash<float>{}(mipBias));
        core::HashUtility::HashCombine(seed, core::THash<bool>{}(minFilter));
        core::HashUtility::HashCombine(seed, core::THash<bool>{}(magFilter));
        core::HashUtility::HashCombine(seed, core::THash<bool>{}(mipFilter));
        core::HashUtility::HashCombine(seed, core::THash<uint8_t>{}(static_cast<uint8_t>(addressU)));
        core::HashUtility::HashCombine(seed, core::THash<uint8_t>{}(static_cast<uint8_t>(addressV)));
        core::HashUtility::HashCombine(seed, core::THash<uint8_t>{}(static_cast<uint8_t>(addressW)));
        core::HashUtility::HashCombine(seed, core::THash<uint8_t>{}(static_cast<uint8_t>(reductionType)));
        return seed;
    }
};

class DrawContext;

class Texture : public core::ReferenceObject
{
public:
    struct SetupParameter
    {
        SamplerInfo samplerInfo = {};
        TextureInfo textureInfo = {};
        void* textureData = nullptr;
        bool write = false;
    };

    static Texture* Create(DrawContext& context, const SetupParameter& param);

    Texture();
    virtual ~Texture();
    bool Write(DrawContext& context, const TextureInfo& srcInfo, void* srcData);

    const rhi::TexturePtr& GetTexture() const;
    void SetTexture(const rhi::TexturePtr& texture);

    const rhi::SamplerPtr& GetSampler() const;
    void SetSampler(const rhi::SamplerPtr& sampler);
private:
    static rhi::TexturePtr CreateTexture(DrawContext& context, const TextureInfo& textureInfo);
    static rhi::SamplerPtr CreateSampler(DrawContext& context, const SamplerInfo& samplerInfo);

private:
    rhi::TexturePtr m_textureHandle;
    rhi::SamplerPtr m_samplerHandle;
};
using TexturePtr = core::TReferencePtr<Texture>;

}
}
