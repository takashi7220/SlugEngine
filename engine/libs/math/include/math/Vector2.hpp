#pragma once

#include "math/Math.hpp"

namespace slug 
{
namespace math
{

struct Vector2
{
    ValueType x;
    ValueType y;

    Vector2()
        : x(0)
        , y(0)
    {
    }

    Vector2(ValueType _x, ValueType _y) 
    {
        x = _x;
        y = _y;
    }

    Vector2(const Vector2& v0)
    {
        x = v0.x;
        y = v0.y;
    }

    static Vector2 Zero()
    {
        return Vector2(0, 0);
    }

    static Vector2 One()
    {
        return Vector2(1, 1);
    }

    bool operator==(const Vector2& v0) const
    {
        return math::TEqual(x, v0.x) && math::TEqual(y, v0.y);
    }

    Vector2 operator+(const Vector2& v0) const
    {
        return Vector2(x + v0.x, y + v0.y);
    }

    Vector2 operator-(const Vector2& v0) const
    {
        return Vector2(x - v0.x, y - v0.y);
    }

    Vector2 operator*(const Vector2& v0) const
    {
        return Vector2(x * v0.x, y * v0.y);
    }

    Vector2 operator/(const Vector2& v0) const
    {
        return Vector2(x / v0.x, y / v0.y);
    }

    Vector2 operator+(ValueType v0) const
    {
        return Vector2(x + v0, y * v0);
    }

    Vector2 operator-(ValueType v0) const
    {
        return Vector2(x - v0, y - v0);
    }

    Vector2 operator*(ValueType v0) const
    {
        return Vector2(x * v0, y * v0);
    }

    Vector2 operator/(ValueType v0) const
    {
        return Vector2(x / v0, y / v0);
    }

    void operator+=(const Vector2& v0)
    {
        x += v0.x;
        y += v0.y;
    }

    void operator-=(const Vector2& v0)
    {
        x -= v0.x;
        y -= v0.y;
    }

    void operator*=(const Vector2& v0)
    {
        x *= v0.x;
        y *= v0.y;
    }

    void operator/=(const Vector2& v0)
    {
        x /= v0.x;
        y /= v0.y;
    }

    void operator+=(ValueType v0)
    {
        x += v0;
        y += v0;
    }

    void operator-=(ValueType v0)
    {
        x -= v0;
        y -= v0;
    }

    void operator*=(ValueType v0)
    {
        x *= v0;
        y *= v0;
    }

    void operator/=(ValueType v0)
    {
        x /= v0;
        y /= v0;
    }

    ValueType Dot(const Vector2& v0) const
    {
        return x * v0.x + y * v0.y;
    }

    ValueType Length() const
    {
        return math::TSqrt<ValueType>(Dot(*this));
    }

    Vector2 Normalize() const
    {
        ValueType l = Length();
        if (math::TAbs(l) > math::TEpsilon<ValueType>())
        {
            return (*this / l);
        }
        return Vector2::Zero();
    }

    void SelfNormalize()
    {
        *this = Normalize();
    }

    ValueType Cross(const Vector2& v0) const
    {
        return x * v0.y - v0.x * y;
    }
};
}
}
