#pragma once

#include "math/Math.hpp"

namespace slug::math
{

struct Vector3
{
    ValueType x;
    ValueType y;
    ValueType z;

    Vector3()
    {
        x = 0;
        y = 0;
        z = 0;
    }

    Vector3(ValueType x, ValueType y, ValueType z)
    {
        this->x = x;
        this->y = y;
        this->z = z;
    }

    Vector3(const Vector3& v0)
    {
        this->x = v0.x;
        this->y = v0.y;
        this->z = v0.z;
    }

    static Vector3 Zero()
    {
        return Vector3(0, 0, 0);
    }

    static Vector3 One()
    {
        return Vector3(1, 1, 1);
    }

    bool operator==(const Vector3& v0) const
    {
        return math::TEqual(x, v0.x) && math::TEqual(y, v0.y) && math::TEqual(z, v0.z);
    }

    Vector3 operator+(const Vector3& v0) const
    {
        return Vector3(x + v0.x, y + v0.y, z + v0.z);
    }

    Vector3 operator-(const Vector3& v0) const
    {
        return Vector3(x - v0.x, y - v0.y, z - v0.z);
    }

    Vector3 operator*(const Vector3& v0) const
    {
        return Vector3(x * v0.x, y * v0.y, z * v0.z);
    }

    Vector3 operator/(const Vector3& v0) const
    {
        return Vector3(x / v0.x, y / v0.y, z / v0.z);
    }

    Vector3 operator+(ValueType v0) const
    {
        return Vector3(x + v0, y + v0, z + v0);
    }

    Vector3 operator-(ValueType v0) const
    {
        return Vector3(x - v0, y - v0, z - v0);
    }

    Vector3 operator*(ValueType v0) const
    {
        return Vector3(x * v0, y * v0, z * v0);
    }

    Vector3 operator/(ValueType v0) const
    {
        return Vector3(x / v0, y / v0, z / v0);
    }

    void operator+=(const Vector3& v0)
    {
        *this = *this + v0;
    }

    void operator-=(const Vector3& v0)
    {
        *this = *this - v0;
    }

    void operator*=(const Vector3& v0)
    {
        *this = *this * v0;
    }

    void operator/=(const Vector3& v0)
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


    ValueType Dot(const Vector3& v0) const
    {
        return x * v0.x + y * v0.y + z * v0.z;
    }

    ValueType Length() const
    {
        return math::TSqrt(Dot(*this));
    }

    ValueType LengthSqr() const
    {
        return Dot(*this);
    }

    Vector3 Normalize() const
    {
        ValueType l = Length();
        if (math::TAbs(l) > math::TEpsilon<ValueType>())
        {
            return *this / l;
        }
        return Vector3::Zero();
    }

    void SelfNormalize()
    {
        *this = Normalize();
    }

    Vector3 Cross(const Vector3& v0) const
    {
        return Vector3(
            y * v0.z - v0.y * z,
            z * v0.x - v0.z * x,
            x * v0.y - v0.x * y
        );
    }
};

}
