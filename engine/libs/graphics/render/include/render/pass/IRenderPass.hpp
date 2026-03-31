#pragma once

#include "render/pipeline/PipelineInfo.hpp"
#include "core/Function.hpp"
#include "render/context/BindingCache.hpp"

namespace slug
{
namespace render
{
class IRenderPass : public core::SObject
{
public:
    struct SetupParam
    {

    };

    struct InitializeParam
    {
        core::TFunctionObject<void(const SetupParam&)> setupFunc = {};
        core::TFunctionObject<void(const PipelineInfo&)> prepareFunc = {};
        core::TFunctionObject<void(DrawContext&, const PipelineInfo&)> drawFunc = {};
        core::TFunctionObject<void(DrawContext&, const PipelineInfo&, BindingCachePtr&)> bindingFunc = {};
    };

public:
    void Initialize(const InitializeParam& param);
    virtual void Setup(const IRenderPass::SetupParam& setup) = 0;
    virtual void Prepare(const PipelineInfo& pipelineInfo) = 0;
    virtual void Draw(DrawContext& context, const PipelineInfo& pipelineInfo) = 0;
    virtual void GetBinding(DrawContext& context, const PipelineInfo& pipelineInfo, BindingCachePtr& bindingCache) = 0;
protected:
    InitializeParam m_param;
};
using IRenderPassPtr = core::TReferencePtr<IRenderPass>;

}
}
