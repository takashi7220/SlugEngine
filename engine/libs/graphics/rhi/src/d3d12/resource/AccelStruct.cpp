#pragma once

#include "rhi/d3d12/resource/AccelStruct.hpp"

namespace slug
{
namespace rhi
{
namespace d3d12
{
AccelStruct::AccelStruct(const Context& context)
    :m_context(context)
{    
}

AccelStruct::~AccelStruct()
{
}

void AccelStruct::CreateSRV(size_t descriptor) const
{
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc;
    srvDesc.Format = DXGI_FORMAT_UNKNOWN;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_RAYTRACING_ACCELERATION_STRUCTURE;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.RaytracingAccelerationStructure.Location = dataBuffer->gpuVirtualAddress;

    m_context.device->CreateShaderResourceView(nullptr, &srvDesc, { descriptor });
}

Object AccelStruct::GetNativeObject(ObjectType objectType)
{
    if (dataBuffer)
    {
        return dataBuffer->GetNativeObject(objectType);
    }
    return nullptr;
}

const rt::AccelStructDesc& AccelStruct::GetDesc() const
{
    return desc;
}

bool AccelStruct::isCompacted() const
{
    return compacted;
}

uint64_t AccelStruct::GetDeviceAddress() const
{
    return dataBuffer->gpuVirtualAddress;
}

}
}
}
