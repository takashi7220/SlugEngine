#pragma once

#include "math/Vector3.hpp"
#include "math/simd/SimdVector.hpp"

namespace slug::math
{

struct alignas(16) Vector4
{
    union
    {
        struct
        {
            float x;
            float y;
            float z;
            float w;
        };
        SimdVector value;
    };

    Vector4()
        : x(0.0f), y(0.0f), z(0.0f), w(0.0f)
    {}

    Vector4(const SimdVector& v)
        : value(v)
    {
    }

    Vector4(ValueType x, ValueType y, ValueType z, ValueType w)
        : x(x), y(y), z(z), w(w)
    {
    }

    Vector4(const Vector4& v0)
        : value(v0.value)
    {}

    Vector4(const Vector3& v0)
        : x(v0.x), y(v0.y), z(v0.z), w(1.0f)
    {
    }

    Vector4(const Vector3& v0, ValueType v1)
        : x(v0.x), y(v0.y), z(v0.z), w(v1)
    {
    }

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
        return simd_vector::Equal(value, v0.value);
    }

    Vector4 operator+(const Vector4& v0) const
    {
        return Vector4(simd_vector::Add(value, v0.value));
    }

    Vector4 operator-(const Vector4& v0) const
    {
        return Vector4(simd_vector::Sub(value, v0.value));
    }

    Vector4 operator*(const Vector4& v0) const
    {
        return Vector4(simd_vector::Mul(value, v0.value));
    }

    Vector4 operator/(const Vector4& v0) const
    {
        return Vector4(simd_vector::Div(value, v0.value));
    }

    Vector4 operator+(ValueType v0) const
    {
        return Vector4(x + v0, y + v0, z + v0, w + v0);
    }

    Vector4 operator-(ValueType v0) const
    {
        return Vector4(x - v0, y - v0, z - v0, w - v0);
    }

    Vector4 operator*(ValueType v0) const
    {
        return Vector4(x * v0, y * v0, z * v0, w * v0);
    }

    Vector4 operator/(ValueType v0) const
    {
        return Vector4(x / v0, y / v0, z / v0, w / v0);
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
        return x * v0.x + y * v0.y + z * v0.z + w * v0.w;
    }

    ValueType DotXYZ(const Vector4& v0) const
    {
        return x * v0.x + y * v0.y + z * v0.z;
    }

    ValueType Length() const
    {
        return math::TSqrt(x * x + y * y + z * z + w * w);
    }

    ValueType LengthXYZ() const
    {
        return math::TSqrt(x * x + y * y + z * z);
    }

    Vector4 Normalize() const
    {
        return simd_vector::Normalize(value);
    }

    Vector4 NormalizeXYZ() const
    {
        return simd_vector::NormalizeXYZ(value);
    }

    void SelfNormalize()
    {
        *this = Normalize();
    }

    void SelfNormalizeXYZ()
    {
        *this = NormalizeXYZ();
    }

    Vector3 Cross(const Vector4& v0) const
    {
        return simd_vector::Cross(value, v0.value);
        //return Vector3(y * v0.z - z * v0.y, z * v0.x - x * v0.z, x * v0.y - y * v0.x);
    }

    Vector3 xyz() const
    {
        return math::Vector3(x, y, z);
    }
};
}
