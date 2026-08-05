#include "render_core/texture/Framebuffer.hpp"
#include "render_core/service/RenderService.hpp"

namespace slug::render_core
{
rhi::IFrameBuffer* Framebuffer::GetFramebuffer(RenderService& service, const rhi::TextureSubresourceSet& subresources)
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

        item = service.GetDevice()->CreateFrameBuffer(desc);
    }

    return item;
}

rhi::IFrameBuffer* Framebuffer::GetFramebuffer(RenderService& service, const ViewPtr& view)
{
    if (view != nullptr)
    {
        return GetFramebuffer(service, view->GetSubresource());
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
