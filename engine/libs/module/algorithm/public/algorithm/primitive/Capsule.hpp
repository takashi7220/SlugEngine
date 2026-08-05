#pragma once

#include "math/Vector3.hpp"
#include "math/Matrix4x4.hpp"
#include "math/Matrix3x3.hpp"

namespace slug::algorithm
{

struct Capsule
{
    math::Vector3 begin;
    math::Vector3 end;
    math::ValueType radius;
};

}
