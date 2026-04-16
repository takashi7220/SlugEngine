#pragma once

#include "core/container/Vector.hpp"
#include "rhi/ResourceType.hpp"
#include "rhi/interface/IBuffer.hpp"
#include "rhi/interface/rt/IOpacityMicromap.hpp"
#include "rhi/interface/rt/IAccelStruct.hpp"
#include "rhi/interface/IShader.hpp"
#include "rhi/interface/IBindingLayout.hpp"
#include "rhi/interface/IBindingSet.hpp"

namespace slug::rhi::rt
{
struct PipelineShaderDesc 
{
    core::String exportName;
    ShaderPtr shader;
    BindingLayoutPtr bindingLayout;
};

struct PipelineHitGroupDesc
{
    core::String exportName;
    ShaderPtr closestHitShader;
    ShaderPtr anyHitShader;
    ShaderPtr intersectionShader;
    BindingLayoutPtr bindingLayout;
    bool proceduralPrimitive = false;
};

struct PipelineDesc {
    core::TVector<PipelineShaderDesc> shaders;
    core::TVector<PipelineHitGroupDesc> hitGroups;
    BindingLayoutVector globalBindingLayouts;
    uint32_t maxPayloadSize = 0;
    uint32_t maxAttributeSize = sizeof(float) * 2;
    uint32_t maxRecursionDepth = 1;
    int32_t hlslExtensionsUav = -1;
};

class IPipeline;

class IShaderTable : public IResource 
{
public:
    virtual void SetRayGenerationShader(const char* export_name, IBindingSet* bindings = nullptr) = 0;
    virtual int32_t AddMissShader(const char* export_name, IBindingSet* bindings = nullptr) = 0;
    virtual int32_t AddHitGroup(const char* export_name, IBindingSet* bindings = nullptr) = 0;
    virtual int32_t AddCallableShader(const char* export_name, IBindingSet* bindings = nullptr) = 0;
    virtual void ClearMissShaders() = 0;
    virtual void ClearHitShaders() = 0;
    virtual void ClearCallableShaders() = 0;
    virtual IPipeline* GetPipeline() = 0;
};
using ShaderTablePtr = core::TReferencePtr<IShaderTable>;

class IPipeline : public IResource 
{
public:
    virtual const rt::PipelineDesc& GetDesc() const = 0;
    virtual ShaderTablePtr CreateShaderTable() = 0;
};
using PipelinePtr = core::TReferencePtr<IPipeline>;

struct State
{
    IShaderTable* shaderTable = nullptr;
    BindingSetVector bindings;
};

struct DispatchRaysArguments 
{
    uint32_t width = 1;
    uint32_t height = 1;
    uint32_t depth = 1;
};

}
