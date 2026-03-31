#pragma once

#include "render/utility/View.hpp"
#include "render/texture/Texture.hpp"
#include "rhi/Rhi.hpp"

namespace slug
{
namespace render
{

class DrawContext;

class Framebuffer : public core::ReferenceObject
{
public:
    Framebuffer()
    {}

    virtual rhi::IFrameBuffer* GetFramebuffer(DrawContext& context, const ViewPtr& view);
    virtual void Clear(rhi::ICommandList* commandList, bool clearColor = true, bool clearDepth = true, bool clearStencil = true);

public:
    core::TVector<TexturePtr> renderTargets;
    TexturePtr depthTarget;
    TexturePtr shadingRateSurface;

private:
    rhi::IFrameBuffer* GetFramebuffer(DrawContext& context, const rhi::TextureSubresourceSet& subresouces);

private:
    core::TUnorderedMap<rhi::TextureSubresourceSet, rhi::FrameBufferPtr, rhi::TextureSubresourceSet::Hash> m_framebuffers;
};
using FramebufferPtr = core::TReferencePtr<Framebuffer>;
}
}
