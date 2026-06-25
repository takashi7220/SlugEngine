#include "render_core/texture/Texture.hpp"
#include "core/utility/Hash.hpp"
#include "render_core/service/RenderService.hpp"

namespace slug::render_core
{

TexturePtr Texture::Create(RenderService& service, const SetupParameter& param)
{
    if (param.textureData == nullptr)
    {
        return nullptr;
    }


    TexturePtr texture(new Texture());
    texture->SetTexture(CreateTexture(service, param.textureInfo));
    texture->SetSampler(CreateSampler(service, param.samplerInfo));
    if (param.write)
    {
        texture->Write(service, param.textureInfo, param.textureData);
    }
    return texture;
}

Texture::Texture()
{

}

Texture::~Texture()
{
}

bool Texture::Write(RenderService& service,const TextureInfo& srcInfo, void* srcData)
{
    const rhi::CommandListPtr& commandList = service.GetCommandList(RenderService::CommandListType::Resource);
    commandList->BeginTrackingTextureState(m_textureHandle, rhi::AllSubresources, rhi::ResourceStates::Common);
    for (uint32_t arraySlice = 0; arraySlice < srcInfo.arraySize; arraySlice++)
    {
        for (uint32_t mipLevel = 0; mipLevel < srcInfo.mipLevels; mipLevel++)
        {
            const render_core::TextureSubresourceInfo& subresourceInfo = srcInfo.arraySubresources[arraySlice].mipmapSubresources[mipLevel];

            commandList->WriteTexture(m_textureHandle, arraySlice, mipLevel, static_cast<const char*>(srcData) + subresourceInfo.dataOffset, subresourceInfo.rowPitch);
        }
    }
    commandList->SetPermanentTextureState(m_textureHandle, rhi::ResourceStates::ShaderResource);
    commandList->CommitBarriers();
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

rhi::TexturePtr Texture::CreateTexture(RenderService& service, const TextureInfo& textureInfo)
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

    TexturePoolPtr& resourcePool = service.GetResourcePool().GetTexturePool();
    uint64_t key = core::HashUtility::ConvertStringToHash64(textureInfo.name);
    rhi::TexturePtr texture = nullptr;
    if (resourcePool->Get(key, texture))
    {
        return texture;
    }

    if (texture == nullptr)
    {
        texture = service.GetDevice()->CreateTexture(desc);
        resourcePool->Add(key, texture);
    }
    return texture;
}

rhi::SamplerPtr Texture::CreateSampler(RenderService& service, const SamplerInfo& samplerInfo)
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

    SamplerPoolPtr& resourcePool = service.GetResourcePool().GetSamplerPool();
    uint64_t key = samplerInfo.GetHash();
    rhi::SamplerPtr sampler = nullptr;
    if (resourcePool->Get(key, sampler))
    {
        return sampler;
    }

    if (sampler == nullptr)
    {
        sampler = service.GetDevice()->CreateSampler(desc);
        resourcePool->Add(key, sampler);
    }

    return sampler;
}

}
