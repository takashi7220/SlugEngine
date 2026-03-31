#include "rhi/d3d12/resource/RootSignature.hpp"
#include "rhi/d3d12/resource/DeviceResources.hpp"
#include "rhi/common/Resource.hpp"

namespace slug
{
namespace rhi
{
namespace d3d12
{
RootSignature::RootSignature(DeviceResources& resources)
    : m_resources(resources)
{
}

RootSignature::~RootSignature() 
{
    const auto it = m_resources.rootsignatureCache.find(hash);
    if (it != m_resources.rootsignatureCache.end()) 
    {
        m_resources.rootsignatureCache.erase(it);
    }
}

Object RootSignature::GetNativeObject(ObjectType objectType)
{
    switch (objectType) 
    {
    case object_types::D3D12_RootSignature:
        return Object(handle.get());
    default:
        return nullptr;
    }
}
}
}
}