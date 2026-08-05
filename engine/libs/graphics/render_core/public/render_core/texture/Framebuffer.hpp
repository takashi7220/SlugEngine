#pragma once

#include "render_core/utility/View.hpp"
#include "render_core/texture/Texture.hpp"
#include "rhi/Rhi.hpp"

namespace slug::render_core
{

class DrawContext;

class Framebuffer : public core::ReferenceObject
{
public:
    Framebuffer()
    {}

    virtual rhi::IFrameBuffer* GetFramebuffer(RenderService& service, const ViewPtr& view);
    virtual void Clear(rhi::ICommandList* commandList, bool clearColor = true, bool clearDepth = true, bool clearStencil = true);

public:
    core::TVector<TexturePtr> renderTargets;
    TexturePtr depthTarget;
    TexturePtr shadingRateSurface;

private:
    rhi::IFrameBuffer* GetFramebuffer(RenderService& service, const rhi::TextureSubresourceSet& subresouces);

private:
    core::TUnorderedMap<rhi::TextureSubresourceSet, rhi::FrameBufferPtr, rhi::TextureSubresourceSet::Hash> m_framebuffers;
};
using FramebufferPtr = core::TReferencePtr<Framebuffer>;
}
