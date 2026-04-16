#pragma once

#include "rhi/ResourceType.hpp"

namespace slug::rhi
{
struct BufferDesc
{
    uint64_t byteSize = 0;
    uint32_t structStride = 0;
    core::String debugName = "";
    FormatType format = FormatType::UNKNOWN;
    bool useUavs = false;
    bool useTypedViews = false;
    bool useRawViews = false;
    bool useVertexBuffer = false;
    bool useIndexBuffer = false;
    bool useConstantBuffer = false;
    bool drawIndirectArgs = false;
    bool accelStructBuildInput = false;
    bool accelStructStorage = false;
    bool shaderBindingTable = false;
    bool volatileBuffer = false;
    bool virtualBuffer = false;
    ResourceStatesBitset initialState = {};
    bool keepInitialState = false;
    CpuAccessMode cpuAccess = CpuAccessMode::None;
    SharedResourceFlagsBitset sharedResourceFlags = SharedResourceFlags::None;
    uint32_t maxVersions = 0;
};

struct BufferRange
{
    uint64_t byteOffset = 0;
    uint64_t byteSize = 0;

    BufferRange Resolve(const BufferDesc& desc) const
    {
        BufferRange result;
        result.byteOffset = math::TMin(byteOffset, desc.byteSize);
        if (byteSize == 0)
        {
            result.byteSize = desc.byteSize - result.byteOffset;
        }
        else
        {
            result.byteSize = math::TMin(byteSize, desc.byteSize - result.byteOffset);
        }
        return result;
    }
    
    constexpr bool CheckEntireBuffer(const BufferDesc& desc) const
    {
        return (byteOffset == 0) && (byteSize == ~0ull || byteSize == desc.byteSize);
    }

    constexpr bool operator==(const BufferRange& other) const
    {
        return byteOffset == other.byteOffset && byteSize == other.byteSize;
    }
};
static const BufferRange EntireBuffer = BufferRange(0, ~0ull);

class IBuffer : public IResource
{
public:
    virtual const BufferDesc& GetDesc() const = 0;
};

using BufferPtr = core::TReferencePtr<IBuffer>;
}
