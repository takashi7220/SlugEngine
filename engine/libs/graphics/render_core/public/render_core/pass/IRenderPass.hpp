#pragma once

#include "render_core/pipeline/PipelineInfo.hpp"
#include "core/Function.hpp"
#include "render_core/binding/BindingCache.hpp"
#include "render_core/Define.hpp"

namespace slug::render_core
{
class IRenderPass : public RenderResource
{
public:
    SLUG_DECLARE_SOBJECT_TYPEINFO(IRenderPass, RenderResource)

    struct SetupParam
    {

    };

    struct InitializeParam
    {
        core::TFunctionObject<void(const SetupParam&)> setupFunc = {};
        core::TFunctionObject<void(const PipelineInfo&)> prepareFunc = {};
        core::TFunctionObject<void(RenderService&, const PipelineInfo&)> drawFunc = {};
        core::TFunctionObject<void(RenderService&, const PipelineInfo&, BindingCachePtr&)> bindingFunc = {};
    };

public:
    void Initialize(const InitializeParam& param);
    virtual void Setup(const IRenderPass::SetupParam& setup) = 0;
    virtual void Prepare(const PipelineInfo& pipelineInfo) = 0;
    virtual void Draw(RenderService& service, const PipelineInfo& pipelineInfo) = 0;
    virtual void GetBinding(RenderService& service, const PipelineInfo& pipelineInfo, BindingCachePtr& bindingCache) = 0;
protected:
    InitializeParam m_param;
};
using IRenderPassPtr = core::TReferencePtr<IRenderPass>;

}
