#pragma once

#include "rhi/d3d12/resource/DeviceResources.hpp"
#include "rhi/d3d12/resource/RayTracingPipeline.hpp"

namespace slug::rhi::d3d12
{

class ShaderTableState
{
public:
    uint32_t committed_version = 0;
    ID3D12DescriptorHeap* descriptor_heap_srv = nullptr;
    ID3D12DescriptorHeap* descriptor_heap_samplers = nullptr;
    D3D12_DISPATCH_RAYS_DESC dispatch_rays_template = {};
};

class ShaderTable : public rt::IShaderTable
{
public:
    struct Entry
    {
        const void* pShaderIdentifier;
        BindingSetPtr localBindings;
    };

public:
    ShaderTable(const Context& context, RayTracingPipeline* _pipeline);
    uint32_t GetNumEntries() const;
    void SetRayGenerationShader(const char* exportName, IBindingSet* bindings = nullptr) override;
    int32_t AddMissShader(const char* exportName, IBindingSet* bindings = nullptr) override;
    int32_t AddHitGroup(const char* exportName, IBindingSet* bindings = nullptr) override;
    int32_t AddCallableShader(const char* exportName, IBindingSet* bindings = nullptr) override;
    void ClearMissShaders() override;
    void ClearHitShaders() override;
    void ClearCallableShaders() override;
    rt::IPipeline* GetPipeline() override;

private:
    bool VerifyExport(const RayTracingPipeline::ExportTableEntry* pExport, IBindingSet* bindings) const;

public:
    core::TReferencePtr<RayTracingPipeline> pipeline;
    Entry rayGenerationShader = {};
    core::TVector<Entry> missShaders;
    core::TVector<Entry> callableShaders;
    core::TVector<Entry> hitGroups;
    uint32_t version = 0;

private:
    const Context& m_context;
};



}
