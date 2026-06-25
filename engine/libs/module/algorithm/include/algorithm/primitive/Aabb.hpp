#pragma once

#include "math/Vector3.hpp"
#include "math/Matrix4x4.hpp"
#include "math/Matrix3x3.hpp"

namespace slug::algorithm
{
struct Aabb
{
    math::Vector3 max = {};
    math::Vector3 min = {};

    Aabb() = default;

    Aabb(math::Vector3 _min, math::Vector3 _max)
        : max(_max)
        , min(_min)
    {
    }

    Aabb(math::ValueType _min, math::ValueType _max)
    {
        max = math::Vector3 { _max, _max, _max };
        min = math::Vector3 { _min, _min, _min };
    }

    void Merge(const math::Vector3& position)
    {
        max.x = math::TMax(position.x, max.x);
        max.y = math::TMax(position.y, max.y);
        max.z = math::TMax(position.z, max.z);

        min.x = math::TMin(position.x, min.x);
        min.y = math::TMin(position.y, min.y);
        min.z = math::TMin(position.z, min.z);
    }

    void Merge(const Aabb& aabb)
    {
        Merge(aabb.max);
        Merge(aabb.min);
    }

    math::Vector3 Range() const
    {
        return max - min;
    }

    math::ValueType Length() const
    {
        return (max - min).Length();
    }

    bool operator==(const Aabb& aabb)
    {
        bool equal = false;
        equal &= this->max == aabb.max;
        equal &= this->min == aabb.min;
        return equal;
    }

    bool Empty() const
    {
        return max.x < min.x || max.y < min.y || max.z < min.z;
    }

    bool Intersects(const Aabb& a) const
    {
        bool is_intersect = false;
        is_intersect = is_intersect && a.min.x <= max.x;
        is_intersect = is_intersect && a.min.y <= max.y;
        is_intersect = is_intersect && a.min.z <= max.z;

        is_intersect = is_intersect && min.x <= a.max.x;
        is_intersect = is_intersect && min.x <= a.max.x;
        is_intersect = is_intersect && min.x <= a.max.x;

        return is_intersect;
    }

    math::Vector3 Center() const
    {
        return (max + min) * 0.5f;
    }

    math::Vector3 Size() const
    {
        return max - min;
    }

    Aabb operator*(const math::Matrix4x4& mat) const
    {
        Aabb tmp_aabb = *this;
        math::Vector4 tmp_max_vec = math::Vector4(tmp_aabb.max);
        math::Vector4 tmp_min_vec = math::Vector4(tmp_aabb.min);
        tmp_max_vec = mat * tmp_max_vec;
        tmp_min_vec = mat * tmp_min_vec;

        tmp_aabb.max.x = tmp_max_vec.x;
        tmp_aabb.max.y = tmp_max_vec.y;
        tmp_aabb.max.z = tmp_max_vec.z;

        tmp_aabb.min.x = tmp_min_vec.x;
        tmp_aabb.min.y = tmp_min_vec.y;
        tmp_aabb.min.z = tmp_min_vec.z;

        return tmp_aabb;
    }

    Aabb operator*(const math::Matrix3x3& mat) const
    {
        Aabb tmp_aabb = *this;
        tmp_aabb.max = mat * tmp_aabb.max;
        tmp_aabb.min = mat * tmp_aabb.min;
        return tmp_aabb;
    }

    Aabb operator|(const Aabb& a)
    {
        Aabb ret = *this;
        ret.Merge(a);
        return ret;
    }

    Aabb operator|=(const Aabb& a)
    {
        Merge(a);
        return *this;
    }
};

}
