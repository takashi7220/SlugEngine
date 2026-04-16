#pragma once

#include "rhi/ResourceType.hpp"
#include "rhi/interface/IBindingLayout.hpp"
#include "rhi/interface/IShader.hpp"
#include "rhi/interface/IBindingSet.hpp"


namespace slug::rhi
{
struct ComputePipelineDesc
{
    ShaderPtr cs;
    BindingLayoutVector bindingLayouts;
}
;
class IComputePipeline : public IResource 
{
public:
    virtual const ComputePipelineDesc& GetDesc() const = 0;
};
typedef core::TReferencePtr<IComputePipeline> ComputePipelinePtr;

struct DrawArguments
{
    uint32_t vertexCount = 0;
    uint32_t instanceCount = 1;
    uint32_t startIndexLocation = 0;
    uint32_t startVertexLocation = 0;
    uint32_t startInstanceLocation = 0;
};

struct DrawIndirectArguments
{
    uint32_t vertexCount = 0;
    uint32_t instanceCount = 1;
    uint32_t startVertexLocation = 0;
    uint32_t startInstanceLocation = 0;
};

struct DrawIndexedIndirectArguments
{
    uint32_t indexCount = 0;
    uint32_t instanceCount = 1;
    uint32_t startIndexLocation = 0;
    int32_t  baseVertexLocation = 0;
    uint32_t startInstanceLocation = 0;
};

struct ComputeState
{
    IComputePipeline* pipeline = nullptr;
    BindingSetVector bindings;
    IBuffer* indirectParams = nullptr;
};


}
