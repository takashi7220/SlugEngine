#include "core/memory/MemoryUtilities.hpp"
#include "rhi/Rhi.hpp"
#include "rhi/d3d12/D3D12Raytracing.hpp"
#include "rhi/d3d12/D3D12Utility.hpp"
#include "rhi/d3d12/resource/Buffer.hpp"

namespace slug
{
namespace rhi
{
namespace d3d12
{

constexpr void D3D12RaytracingGeometryDesc::Validate()
{
    //const size_t ksize_t = sizeof(D3D12_RAYTRACING_GEOMETRY_TYPE) + sizeof(D3D12_RAYTRACING_GEOMETRY_FLAGS) + math::TMax(sizeof(D3D12_RAYTRACING_GEOMETRY_TRIANGLES_DESC), sizeof(D3D12_RAYTRACING_GEOMETRY_AABBS_DESC));
    //SLORIS_STATIC_ASSERT_MSG(sizeof(D3D12_RAYTRACING_GEOMETRY_DESC) == ksize_t, "");
    SLUG_STATIC_ASSERT_MSG(offsetof(D3D12_RAYTRACING_GEOMETRY_DESC, Type) == offsetof(RaytracingGeometryDesc, type), "");
    SLUG_STATIC_ASSERT_MSG(offsetof(D3D12_RAYTRACING_GEOMETRY_DESC, Flags) == offsetof(RaytracingGeometryDesc, flags), "");
    SLUG_STATIC_ASSERT_MSG(offsetof(D3D12_RAYTRACING_GEOMETRY_DESC, Triangles) == offsetof(RaytracingGeometryDesc, triangles), "");
    SLUG_STATIC_ASSERT_MSG(offsetof(D3D12_RAYTRACING_GEOMETRY_DESC, AABBs) == offsetof(RaytracingGeometryDesc, aabbs), "");
}

void D3D12RaytracingGeometryDesc::SetFlags(D3D12_RAYTRACING_GEOMETRY_FLAGS flags)
{
    m_data.flags = flags;
}

void D3D12RaytracingGeometryDesc::SetTriangles(const D3D12_RAYTRACING_GEOMETRY_TRIANGLES_DESC& triangles)
{
    m_data.type = D3D12_RAYTRACING_GEOMETRY_TYPE_TRIANGLES;
    m_data.triangles = triangles;
}

void D3D12RaytracingGeometryDesc::SetAABBs(const D3D12_RAYTRACING_GEOMETRY_AABBS_DESC& aabbs)
{
    m_data.type = D3D12_RAYTRACING_GEOMETRY_TYPE_PROCEDURAL_PRIMITIVE_AABBS;
    m_data.aabbs = aabbs;
}

void D3D12BuildRaytracingAccelerationStructureInputs::SetGeometryDescCount(uint32_t numDescs)
{
    m_geomDescs.resize(numDescs);
    m_geomDescsPtr.resize(numDescs);
    for (uint32_t i = 0; i < numDescs; ++i)
    {
        m_geomDescsPtr[i] = m_geomDescs.data() + i;
    }
    m_desc.ppGeometryDescs = m_geomDescsPtr.data();
    m_desc.numDescs = numDescs;
    m_desc.descsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY_OF_POINTERS;
}

void D3D12BuildRaytracingAccelerationStructureInputs::SetType(D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE type)
{
    m_desc.type = type;
}

void D3D12BuildRaytracingAccelerationStructureInputs::SetFlags(D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAGS flags)
{
    m_desc.flags = flags;
}

void D3D12BuildRaytracingAccelerationStructureInputs::SetInstanceDescs(D3D12_GPU_VIRTUAL_ADDRESS instanceDescs, UINT numDescs)
{
    m_desc.instanceDescs = instanceDescs;
    m_desc.numDescs = numDescs;
}

D3D12RaytracingGeometryDesc& D3D12BuildRaytracingAccelerationStructureInputs::GetGeometryDesc(uint32_t index)
{
    return m_geomDescs[index];
}

const D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS D3D12BuildRaytracingAccelerationStructureInputs::GetAs()
{
    D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS inputs = {};
    inputs.Type = m_desc.type;
    inputs.Flags = m_desc.flags;
    inputs.NumDescs = m_desc.numDescs;
    inputs.DescsLayout = m_desc.descsLayout;
    inputs.InstanceDescs = m_desc.instanceDescs;
    SLUG_STATIC_ASSERT_MSG(sizeof(BuildRaytracingAccelerationStructure::ppGeometryDescs) == sizeof(BuildRaytracingAccelerationStructure::instanceDescs), "");
    SLUG_STATIC_ASSERT_MSG(sizeof(D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS::ppGeometryDescs) == sizeof(D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS::InstanceDescs), "");
    return inputs;
}

void D3D12Raytracing::FillD3dGeometryTrianglesDesc(D3D12_RAYTRACING_GEOMETRY_TRIANGLES_DESC& outDxrTriangles, const rt::GeometryDesc& geometryDesc, D3D12_GPU_VIRTUAL_ADDRESS transform4x4)
{
    const auto& triangles = geometryDesc.geometryData.triangles;

    if (triangles.indexBuffer)
    {
        outDxrTriangles.IndexBuffer = core::MemoryUtilities::CheckedCast<Buffer*>(triangles.indexBuffer)->gpuVirtualAddress + triangles.indexOffset;
    }
    else
    {
        outDxrTriangles.IndexBuffer = 0;
    }

    if (triangles.vertexBuffer)
    {
        outDxrTriangles.VertexBuffer.StartAddress = core::MemoryUtilities::CheckedCast<Buffer*>(triangles.vertexBuffer)->gpuVirtualAddress + triangles.vertexOffset;
    }
    else
    {
        outDxrTriangles.VertexBuffer.StartAddress = 0;
    }

    outDxrTriangles.VertexBuffer.StrideInBytes = triangles.vertexStride;
    outDxrTriangles.IndexFormat = D3D12Utility::GetDxgiFormatMapping(triangles.indexFormat).srvFormat;
    outDxrTriangles.VertexFormat = D3D12Utility::GetDxgiFormatMapping(triangles.vertexFormat).srvFormat;
    outDxrTriangles.IndexCount = UINT(triangles.indexCount);
    outDxrTriangles.VertexCount = UINT(triangles.vertexCount);
    outDxrTriangles.Transform3x4 = transform4x4;
}

void D3D12Raytracing::FillD3dAABBDesc(D3D12_RAYTRACING_GEOMETRY_AABBS_DESC& outDxrAABB, const rt::GeometryDesc& geometryDesc)
{
    const auto& aabbs = geometryDesc.geometryData.aabbs;

    if (aabbs.buffer)
    {
        outDxrAABB.AABBs.StartAddress = core::MemoryUtilities::CheckedCast<Buffer*>(aabbs.buffer)->gpuVirtualAddress + aabbs.offset;
    }
    else
    {
        outDxrAABB.AABBs.StartAddress = 0;
    }

    outDxrAABB.AABBs.StrideInBytes = aabbs.stride;
    outDxrAABB.AABBCount = aabbs.count;
}

void D3D12Raytracing::FillD3dGeometryDesc(D3D12RaytracingGeometryDesc& outD3dGeometryDesc, const rt::GeometryDesc& geometryDesc, D3D12_GPU_VIRTUAL_ADDRESS transform4x4)
{
    outD3dGeometryDesc.SetFlags((D3D12_RAYTRACING_GEOMETRY_FLAGS)geometryDesc.flags);

    if (geometryDesc.geometryType == rt::GeometryType::Triangles)
    {
        const auto& triangles = geometryDesc.geometryData.triangles;
        if (triangles.opacityMicromap != nullptr || triangles.ommIndexBuffer != nullptr)
        {
            SLUG_LOG(RHILogCategory, core::LogType::Alert, "NotSupported");
        }
        else
        {
            D3D12_RAYTRACING_GEOMETRY_TRIANGLES_DESC dxrTriangles = {};
            FillD3dGeometryTrianglesDesc(dxrTriangles, geometryDesc, transform4x4);
            outD3dGeometryDesc.SetTriangles(dxrTriangles);
        }
    }
    else
    {
        D3D12_RAYTRACING_GEOMETRY_AABBS_DESC dxrAABBs = {};
        FillD3dAABBDesc(dxrAABBs, geometryDesc);
        outD3dGeometryDesc.SetAABBs(dxrAABBs);
    }
}

void D3D12Raytracing::FillAsInputDescForPreBuildInfo(D3D12BuildRaytracingAccelerationStructureInputs& outASInputs, const rt::AccelStructDesc& desc)
{
    D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAGS asFlag = D3D12Utility::ConvertAcceslerationStructureBuildFlags(desc.buildFlags);

    if (desc.isTopLevel)
    {
        outASInputs.SetType(D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL);
        outASInputs.SetFlags(asFlag);
        outASInputs.SetInstanceDescs(D3D12_GPU_VIRTUAL_ADDRESS { 0, }, (UINT)desc.topLevelMaxInstances);
    }
    else
    {
        outASInputs.SetType(D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL);
        outASInputs.SetFlags(asFlag);
        outASInputs.SetGeometryDescCount((UINT)desc.bottomLevelGeometries.size());
        for (uint32_t i = 0; i < desc.bottomLevelGeometries.size(); i++)
        {
            const rt::GeometryDesc& srcDesc = desc.bottomLevelGeometries[i];
            D3D12_GPU_VIRTUAL_ADDRESS transform4x4 = srcDesc.useTransform ? 16 : 0;
            D3D12RaytracingGeometryDesc& geomDesc = outASInputs.GetGeometryDesc(i);
            FillD3dGeometryDesc(geomDesc, srcDesc, transform4x4);
        }
    }
}

}
}
}
