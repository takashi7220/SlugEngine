#pragma once

#include "math/Math.hpp"
#include "math/simd/SimdVector.hpp"

namespace slug::math
{

struct Vector3
{
    float x;
    float y;
    float z;

    Vector3()
        : x(0.0f), y(0.0f), z(0.0f)
    {
    }

    Vector3(ValueType x, ValueType y, ValueType z)
        : x(x), y(y), z(z)
    {
    }

    Vector3(const Vector3& v0)
        : x(v0.x), y(v0.y), z(v0.z)
    {
    }

    Vector3& operator=(const Vector3& v0)
    {
        if (this != &v0)
        {
            x = v0.x;
            y = v0.y;
            z = v0.z;
        }
        return *this;
    }

    Vector3(const SimdVector& v)
    {
        x = simd_vector::GetX(v);
        y = simd_vector::GetY(v);
        z = simd_vector::GetZ(v);
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
        bool nearX = math::TEqual(x, v0.x);
        bool nearY = math::TEqual(y, v0.y);
        bool nearZ = math::TEqual(z, v0.z);
        return nearX && nearY && nearZ;
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
        return math::TSqrt(x * x + y * y + z * z);
    }

    ValueType LengthSqr() const
    {
        return Dot(*this);
    }

    Vector3 Normalize() const
    {
        ValueType length = Length();
        if (length == 0)
        {
            return Vector3::Zero();
        }
        return *this / length;
    }

    void SelfNormalize()
    {
        *this = Normalize();
    }

    Vector3 Cross(const Vector3& v0) const
    {
        return Vector3(y * v0.z - z * v0.y, z * v0.x - x * v0.z, x * v0.y - y * v0.x);
    }
};

}
