#pragma once

#include "rhi/ResourceType.hpp"
#include "rhi/interface/IShader.hpp"
#include "rhi/interface/IBindingLayout.hpp"
#include "rhi/interface/IFrameBuffer.hpp"
#include "rhi/interface/IBuffer.hpp"
#include "rhi/interface/IBindingSet.hpp"

namespace slug
{
namespace rhi
{
struct MeshletPipelineDesc {
    PrimitiveType primitiveType = PrimitiveType::TriangleList;

    ShaderPtr as;
    ShaderPtr ms;
    ShaderPtr ps;

    RenderState renderState;

    BindingLayoutVector bindingLayouts;
};

class IMeshletPipeline : public IResource 
{
public:
    virtual const MeshletPipelineDesc& GetDesc() const = 0;
    virtual const FrameBufferInfo& GetFrameBufferInfo() const = 0;
};
using MeshletPipelinePtr = core::TReferencePtr<IMeshletPipeline>;

struct MeshletState
{
    IMeshletPipeline* pipeline = nullptr;
    IFrameBuffer* framebuffer = nullptr;
    ViewportState viewport;
    Color blendConstantColor{};
    BindingSetVector bindings;
    IBuffer* indirectParams = nullptr;
};

}
}
