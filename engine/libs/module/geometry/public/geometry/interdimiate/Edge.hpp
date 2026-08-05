#pragma once

#include "geometry/interdimiate/GeometryTag.hpp"
#include "core/container/EnumBitset.hpp"

namespace slug::geometry::interdimiate
{
struct Edge
{
    enum class Flags : uint8_t
    {
        None = 0,
        Sharp,
        UVSeam,
        ColorSeam,
        Boundary,
        NonManifold,
        SubdivisionCrease,
        Count
    };

    VertexIndex vertex0 = InvalidIndex;
    VertexIndex vertex1 = InvalidIndex;
    core::TEnumBitset<Flags> flags;
    float creaseSharpness = 0.0f;
};
}
