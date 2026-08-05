#pragma once

#include "geometry/interdimiate/GeometryTag.hpp"

namespace slug::geometry::interdimiate
{
struct Corner
{
    VertexIndex vertexIndex = InvalidIndex;
    uint32_t sourceCornerIndex = InvalidIndex;
};
}
