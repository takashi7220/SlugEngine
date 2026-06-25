#pragma once

#include "rhi/Rhi.hpp"
#include "render_core/culling/IDrawObjectCuller.hpp"
#include "render_core/object/PrimitiveDrawObject.hpp"
#include "render_core/utility/View.hpp"
#include "render_core/ResourceType.hpp"
#include "render_core/texture/Framebuffer.hpp"

namespace slug::render_core
{

struct PipelineInfo
{
    struct DrawSettings
    {
        RasterCullMode cullMode = RasterCullMode::FromObject;
        bool depthWriteEnable = true;
        ComparisonFunc depthFunc = render_core::ComparisonFunc::GreaterOrEqual;
        bool stencilEnable = true;
        uint8_t stencilReadMask = 0;
        StencilOp frontFaceStencil = render_core::StencilOp::Replace;
        StencilOp backFaceStencil = render_core::StencilOp::Replace;
    };

    struct SurfaceSettings
    {
        PrimitiveDrawObjectPtr primitiveDrawObject = nullptr;
        IDrawObjectCullerPtr drawObjectCuller = nullptr;
    };

    struct PostProcessSettings
    {
        ShaderPtr shader = nullptr;
        uint8_t stencilValue = 0;
        size_t width = 0;
        size_t height = 0;
    };

    struct ComputeProcessSettings
    {
        ShaderPtr shader = nullptr;
        uint32_t groupX = 0;
        uint32_t groupY = 0;
        uint32_t groupZ = 0;
    };

    ViewPtr view = {};
    rhi::CommandListPtr commandList = nullptr;
    FramebufferPtr framebuffer = nullptr;
    core::TVector<TexturePtr> readTextures = {};
    rhi::GraphicsState graphicsState = {};
    DrawSettings drawSettings = {};
    PostProcessSettings postprocessSettings = {};
    ComputeProcessSettings computeProcessSettings = {};
    SurfaceSettings surfaceSettings = {};
    core::StringView pipelineName = "";
};
}

