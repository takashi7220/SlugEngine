#pragma once

#include "math/MatrixBase.hpp"
#include "math/Vector4.hpp"
#include "math/simd/SimdMatrix.hpp"

namespace slug::math
{

struct alignas(16) Matrix4x4
{
    union
    {
        float m[16];
        SimdMatrix value;
    };

    Matrix4x4()
        : value(simd_matrix::Zero())
    {}

    Matrix4x4(const SimdMatrix& v)
        : value(v)
    {}

    Matrix4x4(const float v[16])
    {
        simd_matrix::Load(value, v);
    }

    // Row-major initializer list → column-major storage
    Matrix4x4(std::initializer_list<std::initializer_list<ValueType>> list)
    {
        for (size_t i = 0; i < 16; ++i) m[i] = ValueType(0);

        auto rowList = list.begin();
        for (size_t row = 0; row < 4 && rowList != list.end(); ++row, ++rowList)
        {
            auto colList = rowList->begin();
            for (size_t col = 0; col < 4 && colList != rowList->end(); ++col, ++colList)
            {
                m[col * 4 + row] = *colList;
            }
        }
    }

    Matrix4x4(const Matrix4x4& other)
        : value(other.value)
    {}

    Matrix4x4& operator=(const Matrix4x4& other)
    {
        if (this != &other)
        {
            value = other.value;
        }
        return *this;
    }

    ValueType& at(size_t row, size_t col)
    {
        return m[col * 4 + row];
    }

    const ValueType& at(size_t row, size_t col) const
    {
        return m[col * 4 + row];
    }

    static Matrix4x4 Zero()
    {
        return Matrix4x4(simd_matrix::Zero());
    }

    static Matrix4x4 Identity()
    {
        return Matrix4x4(simd_matrix::Identity());
    }

    Matrix4x4 Transpose() const
    {
        return Matrix4x4(simd_matrix::Transpose(value));
    }

    Matrix4x4 Inverse() const
    {
        return Matrix4x4(simd_matrix::Inverse(value));
    }

    float Determinant() const
    {
        const float s0 = m[10] * m[15] - m[14] * m[11];
        const float s1 = m[6] * m[15] - m[14] * m[7];
        const float s2 = m[6] * m[11] - m[10] * m[7];
        const float s3 = m[2] * m[15] - m[14] * m[3];
        const float s4 = m[2] * m[11] - m[10] * m[3];
        const float s5 = m[2] * m[7] - m[6] * m[3];

        return
            m[0] * (m[5] * s0 - m[9] * s1 + m[13] * s2) -
            m[4] * (m[1] * s0 - m[9] * s3 + m[13] * s4) +
            m[8] * (m[1] * s1 - m[5] * s3 + m[13] * s5) -
            m[12] * (m[1] * s2 - m[5] * s4 + m[9] * s5);
    }

    Matrix4x4 Adjoint() const
    {
        Matrix4x4 ret = Matrix4x4::Identity();
        ret.m[0]  = m[5]  * m[10] - m[6]  * m[9];
        ret.m[1]  = m[6]  * m[8]  - m[4]  * m[10];
        ret.m[2]  = m[4]  * m[9]  - m[5]  * m[8];
        ret.m[4]  = m[9]  * m[2]  - m[10] * m[1];
        ret.m[5]  = m[10] * m[0]  - m[8]  * m[2];
        ret.m[6]  = m[8]  * m[1]  - m[9]  * m[0];
        ret.m[8]  = m[1]  * m[6]  - m[2]  * m[5];
        ret.m[9]  = m[2]  * m[4]  - m[0]  * m[6];
        ret.m[10] = m[0]  * m[5]  - m[1]  * m[4];
        return ret;
    }

    bool operator==(const Matrix4x4& v0) const
    {
        return simd_matrix::Equal(value, v0.value);
    }

    Matrix4x4 operator+(const Matrix4x4& v0) const
    {
        return Matrix4x4(simd_matrix::Add(value, v0.value));
    }

    Matrix4x4 operator-(const Matrix4x4& v0) const
    {
        return Matrix4x4(simd_matrix::Sub(value, v0.value));
    }

    Matrix4x4 operator*(const Matrix4x4& v0) const
    {
        return Matrix4x4(simd_matrix::Mul(value, v0.value));
    }

    Matrix4x4 operator+(ValueType v0) const
    {
        return Matrix4x4(simd_matrix::Add(value, v0));
    }

    Matrix4x4 operator-(ValueType v0) const
    {
        return Matrix4x4(simd_matrix::Sub(value, v0));
    }

    Matrix4x4 operator*(ValueType v0) const
    {
        return Matrix4x4(simd_matrix::Mul(value, v0));
    }

    Matrix4x4 operator/(ValueType v0) const
    {
        return Matrix4x4(simd_matrix::Div(value, v0));
    }

    void operator+=(const Matrix4x4& v0)
    {
        *this = *this + v0;
    }

    void operator-=(const Matrix4x4& v0)
    {
        *this = *this - v0;
    }

    void operator*=(const Matrix4x4& v0)
    {
        *this = *this * v0;
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

    Vector4 operator*(const Vector4& v0) const
    {
        return Vector4(simd_matrix::MulVector(value, v0.value));
    }

    Vector3 GetExtraScale() const
    {
        const float sx = math::TSqrt(m[0] * m[0] + m[1] * m[1] + m[2] * m[2]);
        const float sy = math::TSqrt(m[4] * m[4] + m[5] * m[5] + m[6] * m[6]);
        const float sz = math::TSqrt(m[8] * m[8] + m[9] * m[9] + m[10] * m[10]);
        return Vector3(sx, sy, sz);
    }

    Vector3 GetTranslate() const
    {
        return Vector3(m[3], m[7], m[11]);
    }
};

}
