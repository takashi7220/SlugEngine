#pragma once

#include "math/Vector3.hpp"

namespace slug::math
{

struct Vector4
{
    ValueType x;
    ValueType y;
    ValueType z;
    ValueType w;

    Vector4()
        : x(0), y(0), z(0), w(0)
    {}

    Vector4(ValueType x, ValueType y, ValueType z, ValueType w)
        : x(x), y(y), z(z), w(w)
    {}

    Vector4(const Vector4& v0)
        : x(v0.x), y(v0.y), z(v0.z), w(v0.w)
    {}

    Vector4(const Vector3& v0)
        : x(v0.x), y(v0.y), z(v0.z), w(1.0f)
    {}

    static Vector4 Zero()
    {
        return Vector4(0, 0, 0, 1);
    }

    static Vector4 One()
    {
        return Vector4(1, 1, 1, 1);
    }

    bool operator==(const Vector4& v0) const
    {
        return x == v0.x && y == v0.y && z == v0.z && w == v0.w;
    }

    Vector4 operator+(const Vector4& v0) const
    {
        return Vector4(x + v0.x, y + v0.y, z + v0.z, 1.0f);
    }

    Vector4 operator-(const Vector4& v0) const
    {
        return Vector4(x - v0.x, y - v0.y, z - v0.z, 1.0f);
    }

    Vector4 operator*(const Vector4& v0) const
    {
        return Vector4(x * v0.x, y * v0.y, z * v0.z, 1.0f);
    }

    Vector4 operator/(const Vector4& v0) const
    {
        return Vector4(x / v0.x, y / v0.y, z / v0.z, 1.0f);
    }

    Vector4 operator+(ValueType v0) const
    {
        return Vector4(x + v0, y + v0, z + v0, 1.0f);
    }

    Vector4 operator-(ValueType v0) const
    {
        return Vector4(x - v0, y - v0, z - v0, 1.0f);
    }

    Vector4 operator*(ValueType v0) const
    {
        return Vector4(x * v0, y * v0, z * v0, 1.0f);
    }

    Vector4 operator/(ValueType v0) const
    {
        return Vector4(x / v0, y / v0, z / v0, 1.0f);
    }

    void operator+=(const Vector4& v0)
    {
        *this = *this + v0;
    }
    void operator-=(const Vector4& v0)
    {
        *this = *this - v0;
    }
    void operator*=(const Vector4& v0)
    {
        *this = *this * v0;
    }
    void operator/=(const Vector4& v0)
    {
        *this = *this / v0;
    }

    void operator+=(ValueType v0)
    {
        *this = *this + v0;
    }
    void operator-=(ValueType v0)
    {
        *this = *this - v0;
    }
    void operator*=(ValueType v0)
    {
        *this = *this * v0;
    }
    void operator/=(ValueType v0)
    {
        *this = *this / v0;
    }

    ValueType Dot(const Vector4& v0) const
    {
        return x * v0.x + y * v0.y + z * v0.z;
    }

    ValueType Length() const
    {
        return math::TSqrt(Dot(*this));
    }

    Vector4 Normalize() const
    {
        ValueType l = Length();
        if (math::TAbs(l) > math::TEpsilon<ValueType>())
        {
            return (*this / l);
        }
        return Vector4::Zero();
    }

    void SelfNormalize()
    {
        *this = Normalize();
    }

    Vector3 Cross(const Vector4& v0) const
    {
        return Vector3(
            y * v0.z - v0.y * z,
            z * v0.x - v0.z * x,
            x * v0.y - v0.x * y
        );
    }

    Vector3 xyz() const
    {
        math::Vector3 ret = {};
        ret.x = this->x;
        ret.y = this->y;
        ret.z = this->z;
        return ret;
    }
};
}
