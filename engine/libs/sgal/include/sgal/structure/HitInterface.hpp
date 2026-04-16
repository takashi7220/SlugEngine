#pragma once

#include "math/AABB.hpp"
#include "core/memory/SObject.hpp"

namespace slug::sgal
{

class HitInterface : core::SObject
{
public:
    virtual bool Intersect(const math::AABB& boundingbox) const = 0;
};

class SphereHitInterface : public HitInterface
{
public:
    SphereHitInterface(const math::Vector3 center, float radius)
        : m_center(center)
        , m_radius(radius)
    {
    }

    bool Intersect(const math::AABB& boundingBox) const
    {
        math::Vector3 p;
        p.x = math::TClamp(m_center.x, boundingBox.min.x, boundingBox.max.x);
        p.y = math::TClamp(m_center.y, boundingBox.min.y, boundingBox.max.y);
        p.z = math::TClamp(m_center.z, boundingBox.min.z, boundingBox.max.z);

        math::Vector3 v = p - m_center;
        return v.LengthSqr() <= (m_radius * m_radius);
    }

private:
    math::Vector3 m_center;
    float m_radius;
};

class AABBHitInterface : public HitInterface
{
public:
    AABBHitInterface(const math::AABB aabb)
        : m_aabb(aabb)
    {
    }

    bool Intersect(const math::AABB& boundingBox) const
    {
        return m_aabb.Intersects(boundingBox);
    }

private:
    math::AABB m_aabb;
};

class RayHitInterface : public HitInterface
{
public:
    RayHitInterface(math::Vector3 origin, math::Vector3 dir, float max = 1e30f)
        : m_origin(origin)
        , m_dir(dir)
        , m_max(max)
    {
    }

    bool Intersect(const math::AABB& boundingBox) const
    {

    }

private:
    math::Vector3 m_origin;
    math::Vector3 m_dir;
    float m_max;
};

class ObbHitInterface : public HitInterface
{
public:
    ObbHitInterface()
    {
    }

    bool Intersect(const math::AABB& boundingBox) const
    {
        return m_aabb.Intersects(boundingBox);
    }

private:
    math::AABB m_aabb;
};

}
