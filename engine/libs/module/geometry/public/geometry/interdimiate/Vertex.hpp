#pragma once

#include "geometry/interdimiate/GeometryTag.hpp"
#include "math/Vector3.hpp"

namespace slug::geometry::interdimiate
{
struct Vertex
{
    math::Vector3 position;
    uint32_t sourceVertexIndex = InvalidIndex;
};
}
