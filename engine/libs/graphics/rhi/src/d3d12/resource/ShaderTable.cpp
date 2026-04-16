#include "rhi/d3d12/resource/ShaderTable.hpp"
#include "rhi/d3d12/resource/BindingSet.hpp"
#include "core/memory/MemoryUtilities.hpp"

namespace slug::rhi::d3d12
{
ShaderTable::ShaderTable(const Context& context, RayTracingPipeline* _pipeline)
    : pipeline(_pipeline)
    , m_context(context)
{
}

uint32_t ShaderTable::GetNumEntries() const
{
    return 1 + uint32_t(missShaders.size()) + uint32_t(hitGroups.size()) + uint32_t(callableShaders.size());
}

void ShaderTable::SetRayGenerationShader(const char* exportName, IBindingSet* bindings)
{
    const RayTracingPipeline::ExportTableEntry* pipelineExport = pipeline->GetExport(exportName);

    if (VerifyExport(pipelineExport, bindings))
    {
        rayGenerationShader.pShaderIdentifier = pipelineExport->pShaderIdentifier;
        rayGenerationShader.localBindings = bindings;
        ++version;
    }
}

int32_t ShaderTable::AddMissShader(const char* exportName, IBindingSet* bindings)
{
    const RayTracingPipeline::ExportTableEntry* pipelineExport = pipeline->GetExport(exportName);

    if (VerifyExport(pipelineExport, bindings))
    {
        Entry entry;
        entry.pShaderIdentifier = pipelineExport->pShaderIdentifier;
        entry.localBindings = bindings;
        missShaders.push_back(entry);

        ++version;

        return int32_t(missShaders.size()) - 1;
    }

    return -1;
}

int32_t ShaderTable::AddHitGroup(const char* exportName, IBindingSet* bindings)
{
    const RayTracingPipeline::ExportTableEntry* pipelineExport = pipeline->GetExport(exportName);

    if (VerifyExport(pipelineExport, bindings))
    {
        Entry entry;
        entry.pShaderIdentifier = pipelineExport->pShaderIdentifier;
        entry.localBindings = bindings;
        hitGroups.push_back(entry);

        ++version;

        return int32_t(hitGroups.size()) - 1;
    }

    return -1;
}

int32_t ShaderTable::AddCallableShader(const char* exportName, IBindingSet* bindings)
{
    const RayTracingPipeline::ExportTableEntry* pipelineExport = pipeline->GetExport(exportName);

    if (VerifyExport(pipelineExport, bindings))
    {
        Entry entry;
        entry.pShaderIdentifier = pipelineExport->pShaderIdentifier;
        entry.localBindings = bindings;
        callableShaders.push_back(entry);

        ++version;

        return int32_t(callableShaders.size()) - 1;
    }

    return -1;
}


void ShaderTable::ClearMissShaders()
{
    missShaders.clear();
    ++version;
}


void ShaderTable::ClearHitShaders()
{
    hitGroups.clear();
    ++version;
}

void ShaderTable::ClearCallableShaders()
{
    callableShaders.clear();
    ++version;
}

rt::IPipeline* ShaderTable::GetPipeline()
{
    return pipeline;
}

bool ShaderTable::VerifyExport(const RayTracingPipeline::ExportTableEntry* pExport, IBindingSet* bindings) const
{
    if (!pExport)
    {
        SLUG_ASSERT_MSG(false, "Couldn't find a DXR PSO export with a given name");
        return false;
    }

    if (pExport->bindingLayout && !bindings)
    {
        SLUG_ASSERT_MSG(false, "A shader table entry does not provide required local bindings");
        return false;
    }

    if (!pExport->bindingLayout && bindings)
    {
        SLUG_ASSERT_MSG(false, "A shader table entry provides local bindings, but none are required");
        return false;
    }

    if (bindings && (core::MemoryUtilities::CheckedCast<d3d12::BindingSet*>(bindings)->layout != pExport->bindingLayout))
    {
        SLUG_ASSERT_MSG(false, "A shader table entry provides local bindings that do not match the expected layout");
        return false;
    }

    return true;
}
}
