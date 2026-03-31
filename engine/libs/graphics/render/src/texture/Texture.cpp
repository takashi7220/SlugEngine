#include "render/texture/Texture.hpp"
#include "core/utility/Hash.hpp"
#include "render/context/DrawContext.hpp"

namespace slug
{
namespace render
{

Texture* Texture::Create(DrawContext& context, const SetupParameter& param)
{
    if (param.textureData == nullptr)
    {
        return nullptr;
    }


    TexturePtr texture(new Texture());
    texture->SetTexture(CreateTexture(context, param.textureInfo));
    texture->SetSampler(CreateSampler(context, param.samplerInfo));
    if (param.write)
    {
        texture->Write(context, param.textureInfo, param.textureData);
    }
    return texture;
}

Texture::Texture()
{

}

Texture::~Texture()
{
}

bool Texture::Write(DrawContext& context,const TextureInfo& srcInfo, void* srcData)
{
    context.GetCreateResourceCommand()->BeginTrackingTextureState(m_textureHandle, rhi::AllSubresources, rhi::ResourceStates::Common);
    for (uint32_t arraySlice = 0; arraySlice < srcInfo.arraySize; arraySlice++)
    {
        for (uint32_t mipLevel = 0; mipLevel < srcInfo.mipLevels; mipLevel++)
        {
            const render::TextureSubresourceInfo& subresourceInfo = srcInfo.arraySubresources[arraySlice].mipmapSubresources[mipLevel];

            context.GetCreateResourceCommand()->WriteTexture(m_textureHandle, arraySlice, mipLevel, static_cast<const char*>(srcData) + subresourceInfo.dataOffset, subresourceInfo.rowPitch);
        }
    }
    context.GetCreateResourceCommand()->SetPermanentTextureState(m_textureHandle, rhi::ResourceStates::ShaderResource);
    context.GetCreateResourceCommand()->CommitBarriers();
    return true;
}

const rhi::TexturePtr& Texture::GetTexture() const
{
    return m_textureHandle;
}

const rhi::SamplerPtr& Texture::GetSampler() const
{
    return m_samplerHandle;
}

void Texture::SetSampler(const rhi::SamplerPtr& sampler)
{
    m_samplerHandle = sampler;
}

void Texture::SetTexture(const rhi::TexturePtr& texture)
{
    m_textureHandle = texture;
}

rhi::TexturePtr Texture::CreateTexture(DrawContext& context, const TextureInfo& textureInfo)
{
    rhi::TextureDesc desc;
    desc.width = textureInfo.width;
    desc.height = textureInfo.height;
    desc.depth = textureInfo.depth;
    desc.arraySize = textureInfo.arraySize;
    desc.mipLevels = textureInfo.mipLevels;
    desc.dimension = static_cast<rhi::TextureDimension>(textureInfo.dimension);
    desc.format = static_cast<rhi::FormatType>(textureInfo.format);
    desc.debugName = textureInfo.name;
    desc.renderTarget = textureInfo.useRenderTarget;

    auto service = context.GetResourceService();
    uint64_t key = core::HashUtility::ConvertStringToHash64(textureInfo.name);
    rhi::TexturePtr texture = nullptr;
    if (service->GetTexturePool()->Get(key, texture))
    {
        return texture;
    }

    if (texture == nullptr)
    {
        texture = context.GetDevice()->CreateTexture(desc);
        service->GetTexturePool()->Add(key, texture);
    }
    return texture;
}

rhi::SamplerPtr Texture::CreateSampler(DrawContext& context, const SamplerInfo& samplerInfo)
{
    rhi::SamplerDesc desc;
    desc.borderColor = samplerInfo.borderColor;
    desc.maxAnisotropy = samplerInfo.maxAnisotropy;
    desc.mipBias = samplerInfo.mipBias;
    desc.minFilter = samplerInfo.minFilter;
    desc.magFilter = samplerInfo.magFilter;
    desc.mipFilter = samplerInfo.mipFilter;
    desc.addressU = static_cast<rhi::SamplerAddressMode>(samplerInfo.addressU);
    desc.addressV = static_cast<rhi::SamplerAddressMode>(samplerInfo.addressV);
    desc.addressW = static_cast<rhi::SamplerAddressMode>(samplerInfo.addressW);
    desc.reductionType = static_cast<rhi::SamplerReductionType>(samplerInfo.reductionType);

    auto service = context.GetResourceService();
    uint64_t key = samplerInfo.GetHash();
    rhi::SamplerPtr sampler = nullptr;
    if (service->GetSamplerPool()->Get(key, sampler))
    {
        return sampler;
    }

    if (sampler == nullptr)
    {
        sampler = context.GetDevice()->CreateSampler(desc);
        service->GetSamplerPool()->Add(key, sampler);
    }

    return sampler;
}

}
}
