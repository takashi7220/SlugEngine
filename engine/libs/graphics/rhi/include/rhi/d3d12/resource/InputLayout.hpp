#pragma once

#include "rhi/interface/IInputLayout.hpp"
#include "rhi/common/StateTracking.hpp"
#include "rhi/d3d12/resource/DeviceResources.hpp"

namespace slug::rhi::d3d12
{

class InputLayout : public IInputLayout
{
public:
    uint32_t GetNumAttributes() const override;
    const VertexAttributeDesc* GetAttributeDesc(uint32_t index) const override;

public:
    core::TVector<VertexAttributeDesc> attributes;
    core::TVector<D3D12_INPUT_ELEMENT_DESC> inputElements;
    core::TUnorderedMap<uint32_t, uint64_t> elementStrides;
};

}
