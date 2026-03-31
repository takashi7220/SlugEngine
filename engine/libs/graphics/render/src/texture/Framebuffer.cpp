#include "render/texture/Framebuffer.hpp"
#include "render/context/DrawContext.hpp"

namespace slug
{
namespace render
{
rhi::IFrameBuffer* Framebuffer::GetFramebuffer(DrawContext& context, const rhi::TextureSubresourceSet& subresources)
{
    if (m_framebuffers.find(subresources) != m_framebuffers.end())
    {
        return m_framebuffers[subresources];
    }

    rhi::FrameBufferPtr& item = m_framebuffers[subresources];

    if (!item)
    {
        rhi::FrameBufferDesc desc;
        for (auto renderTarget : renderTargets)
        {
            auto& attachiment = desc.colorAttachments.emplace_back();
            attachiment.texture = renderTarget->GetTexture();
            attachiment.subresources = subresources;
            attachiment.readOnly = false;
        }

        if (depthTarget)
        {
            auto& attachiment = desc.colorAttachments.emplace_back();
            attachiment.texture = depthTarget->GetTexture();
            attachiment.subresources = subresources;
            attachiment.readOnly = false;
        }

        if (shadingRateSurface)
        {
            auto& attachiment = desc.colorAttachments.emplace_back();
            attachiment.texture = shadingRateSurface->GetTexture();
            attachiment.subresources = subresources;
            attachiment.readOnly = false;
        }

        item = context.GetDevice()->CreateFrameBuffer(desc);
    }

    return item;
}

rhi::IFrameBuffer* Framebuffer::GetFramebuffer(DrawContext& context, const ViewPtr& view)
{
    if (view != nullptr)
    {
        return GetFramebuffer(context, view->GetSubresource());
    }
    return nullptr;
}

void Framebuffer::Clear(rhi::ICommandList* commandList, bool clearColor, bool clearDepth, bool clearStencil)
{
    if (clearColor)
    {
        for (uint32_t i = 0; i < renderTargets.size(); i++)
        {
            rhi::TextureDesc desc = renderTargets.at(i)->GetTexture()->GetDesc();
            if (desc.useClearValue)
            {
                commandList->ClearTextureFloat(renderTargets.at(i)->GetTexture(), rhi::AllSubresources, desc.clearColor);
            }
        }
    }

    if (depthTarget)
    {
        rhi::TextureDesc desc = depthTarget->GetTexture()->GetDesc();
        if (desc.useClearValue)
        {
            commandList->ClearDepthStencilTexture(depthTarget->GetTexture(), rhi::AllSubresources, clearDepth, desc.clearDepth, clearStencil, desc.clearStencil);
        }
    }
}

}
}
