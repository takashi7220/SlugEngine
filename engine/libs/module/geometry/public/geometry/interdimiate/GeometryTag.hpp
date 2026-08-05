#pragma once

#include "core/Define.hpp"
#include "core/NumericLimits.hpp"

namespace slug::geometry::interdimiate
{

constexpr uint32_t InvalidIndex = std::numeric_limits<uint32_t>::max();
using VertexIndex = uint32_t;
using CornerIndex = uint32_t;
using EdgeIndex = uint32_t;
using TriangleIndex = uint32_t;
using PolygonIndex = uint32_t;
using PolygonGroupIndex = uint32_t;
using MaterialIndex = uint32_t;
using JointIndex = uint32_t;

}
