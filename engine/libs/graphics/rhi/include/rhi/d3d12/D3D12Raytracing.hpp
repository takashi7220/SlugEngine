#pragma once

#include <d3d12.h>
#include "rhi/ResourceType.hpp"
#include "rhi/d3d12/interface/IDescriptorHeap.hpp"

namespace slug
{
namespace rhi
{
namespace d3d12
{

class D3D12RaytracingGeometryDesc
{
public:
    struct RaytracingGeometryDesc
    {
        D3D12_RAYTRACING_GEOMETRY_TYPE type;
        D3D12_RAYTRACING_GEOMETRY_FLAGS flags;
        union
        {
            D3D12_RAYTRACING_GEOMETRY_TRIANGLES_DESC           triangles;
            D3D12_RAYTRACING_GEOMETRY_AABBS_DESC               aabbs;
        };
    } m_data;
public:

    constexpr void Validate();
    void SetFlags(D3D12_RAYTRACING_GEOMETRY_FLAGS flags);
    void SetTriangles(const D3D12_RAYTRACING_GEOMETRY_TRIANGLES_DESC& triangles);
    void SetAABBs(const D3D12_RAYTRACING_GEOMETRY_AABBS_DESC& aabbs);
};

class D3D12BuildRaytracingAccelerationStructureInputs
{
public:
    struct BuildRaytracingAccelerationStructure
    {
        D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE type;
        D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAGS flags;
        UINT numDescs;
        D3D12_ELEMENTS_LAYOUT descsLayout;

        union
        {
            D3D12_GPU_VIRTUAL_ADDRESS                   instanceDescs;
            const D3D12RaytracingGeometryDesc* const* ppGeometryDescs;
        };
    };

public:

    void SetGeometryDescCount(uint32_t numDescs);
    void SetType(D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE type);
    void SetFlags(D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAGS flags);
    void SetInstanceDescs(D3D12_GPU_VIRTUAL_ADDRESS instanceDescs, UINT numDescs);
    D3D12RaytracingGeometryDesc& GetGeometryDesc(uint32_t index);
    const D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS GetAs();
public:
    core::TVector<D3D12RaytracingGeometryDesc> m_geomDescs = {};
    core::TVector<D3D12RaytracingGeometryDesc*> m_geomDescsPtr = {};
    BuildRaytracingAccelerationStructure m_desc = {};
};


class D3D12Raytracing
{
public:
    static void FillD3dGeometryTrianglesDesc(D3D12_RAYTRACING_GEOMETRY_TRIANGLES_DESC& outDxrTriangles, const rt::GeometryDesc& geometryDesc, D3D12_GPU_VIRTUAL_ADDRESS transform4x4);
    static void FillD3dAABBDesc(D3D12_RAYTRACING_GEOMETRY_AABBS_DESC& outDxrAABB, const rt::GeometryDesc& geometryDesc);
    static void FillD3dGeometryDesc(D3D12RaytracingGeometryDesc& outD3dGeometryDesc, const rt::GeometryDesc& geometryDesc, D3D12_GPU_VIRTUAL_ADDRESS transform4x4);
    static void FillAsInputDescForPreBuildInfo(D3D12BuildRaytracingAccelerationStructureInputs& outASInputs, const rt::AccelStructDesc& desc);
};

}
}
}
