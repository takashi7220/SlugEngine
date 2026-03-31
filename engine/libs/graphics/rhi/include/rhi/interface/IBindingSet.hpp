#pragma once

#include "rhi/ResourceType.hpp"
#include "rhi/interface/ITexture.hpp"
#include "rhi/interface/IBuffer.hpp"
#include "rhi/interface/ISampler.hpp"
#include "rhi/interface/rt/IAccelStruct.hpp"
#include "rhi/interface/IBindingLayout.hpp"

namespace slug
{
namespace rhi
{

struct BindingSetItem 
{
    IResource* resourcePtr = nullptr;
    uint32_t slot = 0;
    ResourceType type;
    TextureDimension dimension;
    FormatType format;
    uint8_t unused;

    union 
    {
        TextureSubresourceSet subresources;
        BufferRange range;
        uint32_t rawData[2];
    };

    SLUG_STATIC_ASSERT_MSG(sizeof(TextureSubresourceSet) == 16, "sizeof(TextureSubresourceSet) is supposed to be 16 bytes");
    SLUG_STATIC_ASSERT_MSG(sizeof(BufferRange) == 16, "sizeof(BufferRange) is supposed to be 16 bytes");


    BindingSetItem()
        : resourcePtr(nullptr)
        , slot(0)
        , type(ResourceType::ConstantBuffer)
        , dimension(TextureDimension::Unknown)
        , format(FormatType::UNKNOWN)
        , unused(0)
        , range()
    {}

    bool operator ==(const BindingSetItem& b) const
    {
        return resourcePtr == b.resourcePtr
            && slot == b.slot
            && type == b.type
            && dimension == b.dimension
            && format == b.format
            && rawData[0] == b.rawData[0]
            && rawData[1] == b.rawData[1];
    }

    bool operator !=(const BindingSetItem& b) const
    {
        return !(*this == b);
    }

    static BindingSetItem None(uint32_t slot = 0)
    {
        BindingSetItem result;
        result.slot = slot;
        result.type = ResourceType::None;
        result.resourcePtr = nullptr;
        result.format = FormatType::UNKNOWN;
        result.dimension = TextureDimension::Unknown;
        result.rawData[0] = 0;
        result.rawData[1] = 0;
        result.unused = 0;
        return result;
    }

    static BindingSetItem Texture_SRV(uint32_t slot, ITexture* texture, FormatType format = FormatType::UNKNOWN,
        TextureSubresourceSet subresources = AllSubresources, TextureDimension dimension = TextureDimension::Unknown)
    {
        BindingSetItem result;
        result.slot = slot;
        result.type = ResourceType::Texture_SRV;
        result.resourcePtr = texture;
        result.format = format;
        result.dimension = dimension;
        result.subresources = subresources;
        result.unused = 0;
        return result;
    }

    static BindingSetItem Texture_UAV(uint32_t slot, ITexture* texture, FormatType format = FormatType::UNKNOWN,
        TextureSubresourceSet subresources = TextureSubresourceSet(0, 1, 0, TextureSubresourceSet::AllArraySlices),
        TextureDimension dimension = TextureDimension::Unknown)
    {
        BindingSetItem result;
        result.slot = slot;
        result.type = ResourceType::Texture_UAV;
        result.resourcePtr = texture;
        result.format = format;
        result.dimension = dimension;
        result.subresources = subresources;
        result.unused = 0;
        return result;
    }

    static BindingSetItem TypedBuffer_SRV(uint32_t slot, IBuffer* buffer, FormatType format = FormatType::UNKNOWN, BufferRange range = EntireBuffer)
    {
        BindingSetItem result;
        result.slot = slot;
        result.type = ResourceType::TypedBuffer_SRV;
        result.resourcePtr = buffer;
        result.format = format;
        result.dimension = TextureDimension::Unknown;
        result.range = range;
        result.unused = 0;
        return result;
    }

    static BindingSetItem TypedBuffer_UAV(uint32_t slot, IBuffer* buffer, FormatType format = FormatType::UNKNOWN, BufferRange range = EntireBuffer)
    {
        BindingSetItem result;
        result.slot = slot;
        result.type = ResourceType::TypedBuffer_UAV;
        result.resourcePtr = buffer;
        result.format = format;
        result.dimension = TextureDimension::Unknown;
        result.range = range;
        result.unused = 0;
        return result;
    }

    static BindingSetItem ConstantBuffer(uint32_t slot, IBuffer* buffer, BufferRange range = EntireBuffer)
    {
        bool isVolatile = buffer && buffer->GetDesc().volatileBuffer;

        BindingSetItem result;
        result.slot = slot;
        result.type = isVolatile ? ResourceType::VolatileConstantBuffer : ResourceType::ConstantBuffer;
        result.resourcePtr = buffer;
        result.format = FormatType::UNKNOWN;
        result.dimension = TextureDimension::Unknown;
        result.range = range;
        result.unused = 0;
        return result;
    }

    static BindingSetItem Sampler(uint32_t slot, ISampler* sampler)
    {
        BindingSetItem result;
        result.slot = slot;
        result.type = ResourceType::Sampler;
        result.resourcePtr = sampler;
        result.format = FormatType::UNKNOWN;
        result.dimension = TextureDimension::Unknown;
        result.rawData[0] = 0;
        result.rawData[1] = 0;
        result.unused = 0;
        return result;
    }

    static BindingSetItem RayTracingAccelStruct(uint32_t slot, rt::IAccelStruct* as)
    {
        BindingSetItem result;
        result.slot = slot;
        result.type = ResourceType::RayTracingAccelStruct;
        result.resourcePtr = as;
        result.format = FormatType::UNKNOWN;
        result.dimension = TextureDimension::Unknown;
        result.rawData[0] = 0;
        result.rawData[1] = 0;
        result.unused = 0;
        return result;
    }

    static BindingSetItem StructuredBuffer_SRV(uint32_t slot, IBuffer* buffer, FormatType format = FormatType::UNKNOWN, BufferRange range = EntireBuffer)
    {
        BindingSetItem result;
        result.slot = slot;
        result.type = ResourceType::StructuredBuffer_SRV;
        result.resourcePtr = buffer;
        result.format = format;
        result.dimension = TextureDimension::Unknown;
        result.range = range;
        result.unused = 0;
        return result;
    }

    static BindingSetItem StructuredBuffer_UAV(uint32_t slot, IBuffer* buffer, FormatType format = FormatType::UNKNOWN, BufferRange range = EntireBuffer)
    {
        BindingSetItem result;
        result.slot = slot;
        result.type = ResourceType::StructuredBuffer_UAV;
        result.resourcePtr = buffer;
        result.format = format;
        result.dimension = TextureDimension::Unknown;
        result.range = range;
        result.unused = 0;
        return result;
    }

    static BindingSetItem RawBuffer_SRV(uint32_t slot, IBuffer* buffer, BufferRange range = EntireBuffer)
    {
        BindingSetItem result;
        result.slot = slot;
        result.type = ResourceType::RawBuffer_SRV;
        result.resourcePtr = buffer;
        result.format = FormatType::UNKNOWN;
        result.dimension = TextureDimension::Unknown;
        result.range = range;
        result.unused = 0;
        return result;
    }

    static BindingSetItem RawBuffer_UAV(uint32_t slot, IBuffer* buffer, BufferRange range = EntireBuffer)
    {
        BindingSetItem result;
        result.slot = slot;
        result.type = ResourceType::RawBuffer_UAV;
        result.resourcePtr = buffer;
        result.format = FormatType::UNKNOWN;
        result.dimension = TextureDimension::Unknown;
        result.range = range;
        result.unused = 0;
        return result;
    }

    static BindingSetItem PushConstants(uint32_t slot, uint32_t bytesize_t)
    {
        BindingSetItem result;
        result.slot = slot;
        result.type = ResourceType::PushConstants;
        result.resourcePtr = nullptr;
        result.format = FormatType::UNKNOWN;
        result.dimension = TextureDimension::Unknown;
        result.range.byteOffset = 0;
        result.range.byteSize = bytesize_t;
        result.unused = 0;
        return result;
    }

};

SLUG_STATIC_ASSERT_MSG(sizeof(BindingSetItem) == 32, "sizeof(BindingSetItem) is supposed to be 32 bytes");
using BindingSetItemArray = core::TStaticVector<BindingSetItem, MAX_BINDING_PER_LAYOUT>;

struct BindingSetDesc 
{
    BindingSetItemArray bindings;
    bool trackLiveness = true;

    bool operator ==(const BindingSetDesc& b) const
    {
        if (bindings.size() != b.bindings.size())
        {
            return false;
        }

        for (size_t i = 0; i < bindings.size(); ++i) 
        {
            if (bindings[i] != b.bindings[i])
            {
                return false;
            }
        }

        return true;
    }

    bool operator !=(const BindingSetDesc& b) const
    {
        return !(*this == b);
    }

    void AddItem(const BindingSetItem& value)
    {
        bindings.push_back(value);
    }
};

class IBindingSet : public IResource {
public:
    virtual const BindingSetDesc* GetDesc() const = 0;
    virtual IBindingLayout* GetLayout() const = 0;
};
using BindingSetPtr = core::TReferencePtr<IBindingSet>;
using BindingSetVector = core::TStaticVector<IBindingSet*, MAX_BINDING_LAYOUTS>;
}
}