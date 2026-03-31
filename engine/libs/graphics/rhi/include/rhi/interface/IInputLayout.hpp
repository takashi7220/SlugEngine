#pragma once

#include "rhi/ResourceType.hpp"

namespace slug
{
namespace rhi
{
struct VertexAttributeDesc
{
    core::String name = "";
    FormatType format = FormatType::UNKNOWN;
    uint32_t arraySize = 1;
    uint32_t bufferIndex = 0;
    uint64_t offset = 0;
    uint64_t elementStride = 0;
    bool instanced = false;
};

class IInputLayout : public IResource
{
public:
    virtual uint32_t GetNumAttributes() const = 0;
    virtual const VertexAttributeDesc* GetAttributeDesc(uint32_t index) const = 0;
};

using InputLayoutPtr = core::TReferencePtr<IInputLayout>;

}
}
