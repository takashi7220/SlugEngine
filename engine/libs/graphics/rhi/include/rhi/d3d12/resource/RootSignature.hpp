#pragma once

#include "core/container/StaticVector.hpp"
#include "core/memory/ReferencePtr.hpp"
#include "rhi/interface/IBindingLayout.hpp"
#include "rhi/d3d12/D3D12.hpp"
#include "rhi/d3d12/interface/IRootSignature.hpp"
#include "rhi/d3d12/resource/DeviceResources.hpp"
#include <d3d12.h>

namespace slug::rhi::d3d12
{

class RootSignature : public IRootSignature
{
public:
    RootSignature(DeviceResources& resources);
    virtual ~RootSignature() override;
    Object GetNativeObject(ObjectType objectType) override;
public:
    size_t hash = 0;
    core::TStaticVector<core::TPair<BindingLayoutPtr, RootParameterIndex>, MAX_BINDING_LAYOUTS> pipelineLayouts = {};
    core::TReferencePtr<ID3D12RootSignature> handle = nullptr;
    uint32_t pushConstantByteSize = 0;
    RootParameterIndex rootParameterPushConstants = ~0u;
private:
    DeviceResources& m_resources;
};

}
