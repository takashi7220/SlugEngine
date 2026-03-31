#include "rhi/d3d12/resource/ShaderTable.hpp"
#include "rhi/d3d12/resource/RayTracingPipeline.hpp"
#include "rhi/common/Utility.hpp"

namespace slug
{
namespace rhi
{
namespace d3d12
{
RayTracingPipeline::RayTracingPipeline(const Context& context)
    :m_context(context)
{

}

const RayTracingPipeline::ExportTableEntry* RayTracingPipeline::GetExport(const char* name)
{
    const auto exportEntryIt = exports.find(name);
    if (exportEntryIt == exports.end())
    {
        return nullptr;
    }

    return &exportEntryIt->second;
}

uint32_t RayTracingPipeline::GetShaderTableEntrySize() const
{
    uint32_t required_size = D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES + sizeof(UINT64) * maxLocalRootParameters;
    return TAlign(required_size, uint32_t(D3D12_RAYTRACING_SHADER_TABLE_BYTE_ALIGNMENT));
}

const rt::PipelineDesc& RayTracingPipeline::GetDesc() const
{
    return desc;
}

rt::ShaderTablePtr RayTracingPipeline::CreateShaderTable()
{
    return rt::ShaderTablePtr::create(new ShaderTable(m_context, this));
}

}
}
}
