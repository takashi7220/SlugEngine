#pragma once

#include "core/container/Vector.hpp"
#include "rhi/ResourceType.hpp"
#include "rhi/interface/IBuffer.hpp"
#include "rhi/interface/rt/IOpacityMicromap.hpp"

namespace slug
{
namespace rhi
{
namespace rt
{

class IAccelStruct;
using AffineTransform = float[12];

constexpr AffineTransform c_identity_transform =
{
    1.f, 0.f, 0.f, 0.f,
    0.f, 1.f, 0.f, 0.f,
    0.f, 0.f, 1.f, 0.f
};

struct GeometryTriangles
{
    IBuffer* indexBuffer = nullptr;
    IBuffer* vertexBuffer = nullptr;
    FormatType indexFormat = FormatType::UNKNOWN;
    FormatType vertexFormat = FormatType::UNKNOWN;
    uint64_t indexOffset = 0;
    uint64_t vertexOffset = 0;
    uint64_t indexCount = 0;
    uint64_t vertexCount = 0;
    uint64_t vertexStride = 0;
    IOpacityMicromap* opacityMicromap = nullptr;
    IBuffer* ommIndexBuffer = nullptr;
    uint64_t ommIndexBufferOffset = 0;
    FormatType ommIndexFormat = FormatType::UNKNOWN;
    const OpacityMicromapUsageCount* pOmmUsageCounts = nullptr;
    uint32_t numOmmUsageCounts = 0;
};

struct GeometryAABBs
{
    IBuffer* buffer = nullptr;
    IBuffer* unused = nullptr;
    uint64_t offset = 0;
    uint32_t count = 0;
    uint32_t stride = 0;
};

struct GeometryDesc
{
    union GeomTypeUnion
    {
        GeometryTriangles triangles;
        GeometryAABBs aabbs;
    } geometryData;

    bool useTransform = false;
    AffineTransform transform{};
    GeometryFlags flags = GeometryFlags::None;
    GeometryType geometryType = GeometryType::Triangles;
};

struct InstanceDesc
{
    AffineTransform transform = {};
    uint32_t instanceId : 24;
    uint32_t instanceMask : 8;
    uint32_t instanceContributionToHitGroupIndex : 24;
    InstanceFlags flags : 8;
    union 
    {
        IAccelStruct* bottomLevelAs;
        uint64_t blasDviceAddress;
    };
};

struct AccelStructDesc 
{
    size_t topLevelMaxInstances = 0;
    core::TVector<GeometryDesc> bottomLevelGeometries = {};
    AccelStructBuildFlagsBitset buildFlags = AccelStructBuildFlags::None;
    core::String debugName = {};
    bool trackLiveness = true;
    bool isTopLevel = false;
    bool isVirtual = false;

    AccelStructDesc& SetTopLevelMaxInstances(size_t value)
    {
        topLevelMaxInstances = value; isTopLevel = true;
        return *this;
    }
    AccelStructDesc& AddBottomLevelGeometry(const GeometryDesc& value)
    {
        bottomLevelGeometries.push_back(value); isTopLevel = false;
        return *this;
    }
};

class IAccelStruct : public IResource 
{
public:
    virtual const AccelStructDesc& GetDesc() const = 0;
    virtual bool isCompacted() const = 0;
    virtual uint64_t GetDeviceAddress() const = 0;
};
using AccelStructPtr = core::TReferencePtr<IAccelStruct>;

}
}
}
