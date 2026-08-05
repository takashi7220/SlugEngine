#pragma once

#include "geometry/interdimiate/GeometryTag.hpp"
#include "core/container/EnumBitset.hpp"

namespace slug::geometry::interdimiate
{
struct Polygon
{
    enum class Flags : uint8_t
    {
        None = 0,
        Hidden,
        TwoSided,
        Degenerate,
        Hole,
        Generated,
        Count
    };

    uint32_t firstCorner = 0;
    uint32_t cornerCount = 0;
    PolygonGroupIndex polygonGroupIndex = InvalidIndex;
    uint32_t smoothingGroupMask = 0;
    core::TEnumBitset<Flags> flags;
    uint32_t sourcePolygonIndex = InvalidIndex;
};

struct PolygonGroup
{
    core::FixedString<16> name;
    MaterialIndex materialIndex = InvalidIndex;
    core::FixedString<256> sourceMaterialPath;
};
}
