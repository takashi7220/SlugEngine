#pragma once

#include "rhi/ResourceType.hpp"
#include "rhi/interface/IInputLayout.hpp"
#include "rhi/interface/IShader.hpp"
#include "rhi/interface/IBindingLayout.hpp"
#include "rhi/interface/IFrameBuffer.hpp"
#include "rhi/interface/IBuffer.hpp"
#include "rhi/interface/IBindingSet.hpp"

namespace slug
{
namespace rhi
{

struct GraphicsPipelineDesc
{
    PrimitiveType primitiveType = PrimitiveType::TriangleList;
    uint32_t patchControlPoints = 0;
    InputLayoutPtr inputLayout;
    ShaderPtr vs;
    ShaderPtr hs;
    ShaderPtr ds;
    ShaderPtr gs;
    ShaderPtr ps;
    RenderState renderState;
    VariableRateShadingState shadingRateState;
    BindingLayoutVector bindingLayouts;
};

class IGraphicsPipeline : public IResource 
{
public:
    virtual const GraphicsPipelineDesc& GetDesc() const = 0;
    virtual const FrameBufferInfo& GetFrameBufferInfo() const = 0;
    virtual void GetCache(core::TVector<uint8_t>& cache) const = 0;
};

struct VertexBufferBinding
{
    IBuffer* buffer = nullptr;
    uint32_t slot = 0;
    uint64_t offset = 0;
    uint64_t stride = 0;
    rhi::FormatType format = rhi::FormatType::UNKNOWN;
    core::String shaderLayoutName = "";
    bool operator ==(const VertexBufferBinding& b) const
    {
        return buffer == b.buffer
            && slot == b.slot
            && offset == b.offset;
    }
    bool operator !=(const VertexBufferBinding& b) const
    {
        return !(*this == b);
    }
};

struct IndexBufferBinding
{
    IBuffer* buffer = nullptr;
    FormatType format = FormatType::UNKNOWN;
    uint32_t offset = 0;

    bool operator ==(const IndexBufferBinding& b) const
    {
        return buffer == b.buffer
            && format == b.format
            && offset == b.offset;
    }
    bool operator !=(const IndexBufferBinding& b) const
    {
        return !(*this == b);
    }
};

struct GraphicsState
{
    IGraphicsPipeline* pipeline = nullptr;
    IFrameBuffer* framebuffer = nullptr;
    ViewportState viewport;
    Color blendConstantColor{};
    VariableRateShadingState shadingRateState;

    BindingSetVector bindings;

    core::TStaticVector<VertexBufferBinding, MAX_VERTEX_ATTRIBUTES> vertexBuffers;
    IndexBufferBinding indexBuffer;

    IBuffer* indirectParams = nullptr;
};
using GraphicsPipelinePtr = core::TReferencePtr<IGraphicsPipeline>;

}
}
