#pragma once

#include "rhi/interface/rt/IPipeline.hpp"
#include "rhi/d3d12/resource/DeviceResources.hpp"
#include "rhi/d3d12/resource/RootSignature.hpp"

namespace slug::rhi::d3d12
{

class RayTracingPipeline : public rt::IPipeline
{
public:
    struct ExportTableEntry
    {
        IBindingLayout* bindingLayout;
        const void* pShaderIdentifier;
    };

public:
    RayTracingPipeline(const Context& context);

    const ExportTableEntry* GetExport(const char* name);
    uint32_t GetShaderTableEntrySize() const;
    const rt::PipelineDesc& GetDesc() const override;

    rt::ShaderTablePtr CreateShaderTable() override;

public:
    rt::PipelineDesc desc;
    core::TUnorderedMap<IBindingLayout*, RootSignaturePtr> localRootSignatures;
    core::TReferencePtr<RootSignature> globalRootSignature;
    core::TReferencePtr<ID3D12StateObject> pipelineState;
    core::TReferencePtr<ID3D12StateObjectProperties> pipelineInfo;
    core::TUnorderedMap<core::String, ExportTableEntry> exports;
    uint32_t maxLocalRootParameters = 0;

private:
    const Context& m_context;
};

}
