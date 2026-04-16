#pragma once

#include "rhi/ResourceType.hpp"
#include "rhi/interface/ITexture.hpp"

namespace slug::rhi
{
struct FrameBufferAttachment
{
    ITexture* texture = nullptr;
    TextureSubresourceSet subresources = TextureSubresourceSet(0, 1, 0, 1);
    FormatType format = FormatType::UNKNOWN;
    bool readOnly = false;

    bool Valid() const
    {
        return texture != nullptr;
    }
};

struct FrameBufferDesc
{
    core::TStaticVector<FrameBufferAttachment, MAX_RENDER_TARGETS> colorAttachments;
    FrameBufferAttachment depthAttachment;
    FrameBufferAttachment shadingRateAttachment;
};

struct FrameBufferInfo {
    core::TStaticVector<FormatType, MAX_RENDER_TARGETS> colorFormats;
    FormatType depthFormat = FormatType::UNKNOWN;
    uint32_t sampleCount = 1;
    uint32_t sampleQuality = 0;

    FrameBufferInfo() = default;

    FrameBufferInfo(const FrameBufferDesc& desc)
    {
        for (size_t i = 0; i < desc.colorAttachments.size(); i++)
        {
            const FrameBufferAttachment& attachment = desc.colorAttachments[i];
            colorFormats.push_back(attachment.format == FormatType::UNKNOWN && attachment.texture ? attachment.texture->GetDesc().format : attachment.format);
        }

        if (desc.depthAttachment.Valid())
        {
            const TextureDesc& textureDesc = desc.depthAttachment.texture->GetDesc();
            depthFormat = textureDesc.format;
            sampleCount = textureDesc.sampleCount;
            sampleQuality = textureDesc.sampleQuality;
        }
        else if (!desc.colorAttachments.empty() && desc.colorAttachments[0].Valid()) {
            const TextureDesc& textureDesc = desc.colorAttachments[0].texture->GetDesc();
            sampleCount = textureDesc.sampleCount;
            sampleQuality = textureDesc.sampleQuality;
        }
    }

    bool operator==(const FrameBufferInfo& other) const
    {
        return (FormatEquals(colorFormats, other.colorFormats)
            && depthFormat == other.depthFormat
            && sampleCount == other.sampleCount
            && sampleQuality == other.sampleQuality);
    }

    bool operator!=(const FrameBufferInfo& other) const
    {
        return !(*this == other);
    }
private:
    static bool FormatEquals(const core::TStaticVector<FormatType, MAX_RENDER_TARGETS>& a, const core::TStaticVector<FormatType, MAX_RENDER_TARGETS>& b)
    {
        if (a.size() != b.size()) return false;
        for (size_t i = 0; i < a.size(); i++)
        {
            if (a.at(i) != b.at(i))
            {
                return false;
            }
        }
        return true;
    }
};

struct FrameBufferInfoEx : FrameBufferInfo
{
    uint32_t width = 0;
    uint32_t height = 0;

    FrameBufferInfoEx() = default;
    FrameBufferInfoEx(const FrameBufferDesc& desc)
        : FrameBufferInfo(desc)
    {
        if (desc.depthAttachment.Valid())
        {
            const TextureDesc& textureDesc = desc.depthAttachment.texture->GetDesc();
            width = textureDesc.width >> desc.depthAttachment.subresources.baseMipLevel;
            height = textureDesc.height >> desc.depthAttachment.subresources.baseMipLevel;
        }
        else if (!desc.colorAttachments.empty() && desc.colorAttachments[0].Valid())
        {
            const TextureDesc& textureDesc = desc.colorAttachments[0].texture->GetDesc();
            width = textureDesc.width >> desc.colorAttachments[0].subresources.baseMipLevel;
            height = textureDesc.height >> desc.colorAttachments[0].subresources.baseMipLevel;
        }
    }

    Viewport GetViewPort(float minZ = 0.0f, float maxZ = 1.0f) const
    {
        return Viewport(0.0f, float(width), 0.0f, float(height), minZ, maxZ);
    }
};

struct IFrameBuffer : public IResource
{
public:
    virtual const FrameBufferDesc& GetDesc() const = 0;
    virtual const FrameBufferInfoEx& GetFrameBufferInfo() const = 0;
    virtual FrameBufferInfoEx& GetFrameBufferInfo() = 0;
};

using FrameBufferPtr = core::TReferencePtr<IFrameBuffer>;

}
