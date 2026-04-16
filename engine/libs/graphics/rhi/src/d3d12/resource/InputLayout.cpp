#include "rhi/d3d12/resource/InputLayout.hpp"

namespace slug::rhi::d3d12
{

uint32_t InputLayout::GetNumAttributes() const
{
    return uint32_t(attributes.size());
}

const VertexAttributeDesc* InputLayout::GetAttributeDesc(uint32_t index) const
{
    if (index < uint32_t(attributes.size()))
    {
        return &attributes[index];
    }
    else
    {
        return nullptr;
    }
}

}
