#pragma once

#include "rhi/Rhi.hpp"
#include "render/context/IDrawObjectCuller.hpp"
#include "render/context/PrimitiveDrawObject.hpp"
#include "render/utility/View.hpp"
#include "render/ResourceType.hpp"
#include "render/texture/Framebuffer.hpp"

namespace slug::render
{

struct PipelineInfo
{
    struct DrawSettings
    {
        RasterCullMode cullMode = RasterCullMode::FromObject;
        bool depthWriteEnable = true;
        ComparisonFunc depthFunc = render::ComparisonFunc::GreaterOrEqual;
        bool stencilEnable = true;
        uint8_t stencilReadMask = 0;
        StencilOp frontFaceStencil = render::StencilOp::Replace;
        StencilOp backFaceStencil = render::StencilOp::Replace;
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

