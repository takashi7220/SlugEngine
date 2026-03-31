#pragma once

#include "math/MatrixBase.hpp"
#include "math/Vector3.hpp"

namespace slug
{
namespace math
{

struct Matrix3x3
{
    // 列優先（column-major）: m[col * 3 + row]
    ValueType m[9] = {};

    Matrix3x3()
    {
        for (size_t i = 0; i < 9; i++)
        {
            this->m[i] = ValueType(0);
        }
    }

    // 初期化子リストは「行ごと」に受け取り、内部では列優先に格納する
    Matrix3x3(std::initializer_list<std::initializer_list<ValueType>> list)
    {
        // いったん 0 で埋める（不足分にも対応）
        for (size_t i = 0; i < 9; i++) m[i] = ValueType(0);

        auto rowList = list.begin();
        for (size_t row = 0; row < 3 && rowList != list.end(); ++row, ++rowList)
        {
            auto colList = rowList->begin();
            for (size_t col = 0; col < 3 && colList != rowList->end(); ++col, ++colList)
            {
                // 列優先インデクシング
                m[col * 3 + row] = *colList;
            }
        }
    }

    Matrix3x3(const Matrix3x3& other)
    {
        for (size_t i = 0; i < 9; ++i)
        {
            m[i] = other.m[i];
        }
    }

    ValueType& at(size_t i, size_t j)
    {
        return m[j * 3 + i];
    }
    const ValueType& at(size_t i, size_t j) const
    {
        return m[j * 3 + i];
    }

    static Matrix3x3 Zero()
    {
        return Matrix3x3 {};
    }

    static Matrix3x3 Identity()
    {
        return Matrix3x3
        {
            {ValueType(1), ValueType(0), ValueType(0)},
            {ValueType(0), ValueType(1), ValueType(0)},
            {ValueType(0), ValueType(0), ValueType(1)}
        };
    }

    Matrix3x3 Transpose() const
    {
        Matrix3x3 r;
        for (int col = 0; col < 3; ++col)
        {
            for (int row = 0; row < 3; ++row)
            {
                r.m[col * 3 + row] = m[row * 3 + col];
            }
        }
        return r;
    }

    Matrix3x3 Inverse() const
    {
        const ValueType a = m[0], d = m[1], g = m[2];
        const ValueType b = m[3], e = m[4], h = m[5];
        const ValueType c = m[6], f = m[7], i = m[8];

        ValueType det =
            a * (e * i - f * h) -
            b * (d * i - f * g) +
            c * (d * h - e * g);

        if (det == ValueType(0))
        {
            return Zero();
        }

        ValueType invDet = ValueType(1) / det;

        // 返却時も列優先に詰める（行ごとに書いても ctor が列優先へ詰め替える）
        return Matrix3x3 {
            {
                 (e * i - f * h) * invDet,
                -(b * i - h * c) * invDet,
                 (b * f - e * c) * invDet
            },
            {
                -(d * i - f * g) * invDet,
                 (a * i - c * g) * invDet,
                -(a * f - d * c) * invDet
            },
            {
                 (d * h - e * g) * invDet,
                -(a * h - b * g) * invDet,
                 (a * e - b * d) * invDet
            }
        };
    }

    bool operator==(const Matrix3x3& v0) const
    {
        for (int i = 0; i < 9; ++i)
        {
            if (m[i] != v0.m[i])
            {
                return false;
            }
        }
        return true;
    }

    Matrix3x3 operator+(const Matrix3x3& v0) const
    {
        Matrix3x3 result;
        for (int i = 0; i < 9; ++i)
        {
            result.m[i] = m[i] + v0.m[i];
        }
        return result;
    }

    Matrix3x3 operator-(const Matrix3x3& v0) const
    {
        Matrix3x3 result;
        for (int i = 0; i < 9; ++i)
        {
            result.m[i] = m[i] - v0.m[i];
        }
        return result;
    }

    // 列優先想定の行列積: r = (*this) * v0
    Matrix3x3 operator*(const Matrix3x3& v0) const
    {
        Matrix3x3 r;
        for (int col = 0; col < 3; ++col)
        {
            for (int row = 0; row < 3; ++row)
            {
                r.m[col * 3 + row] =
                    m[0 * 3 + row] * v0.m[col * 3 + 0] +
                    m[1 * 3 + row] * v0.m[col * 3 + 1] +
                    m[2 * 3 + row] * v0.m[col * 3 + 2];
            }
        }
        return r;
    }

    Matrix3x3 operator+(ValueType v0) const
    {
        Matrix3x3 result;
        for (int i = 0; i < 9; ++i)
        {
            result.m[i] = m[i] + v0;
        }
        return result;
    }

    Matrix3x3 operator-(ValueType v0) const
    {
        Matrix3x3 result;
        for (int i = 0; i < 9; ++i)
        {
            result.m[i] = m[i] - v0;
        }
        return result;
    }

    Matrix3x3 operator*(ValueType v0) const
    {
        Matrix3x3 result;
        for (int i = 0; i < 9; ++i)
        {
            result.m[i] = m[i] * v0;
        }
        return result;
    }

    Matrix3x3 operator/(ValueType v0) const
    {
        Matrix3x3 result;
        for (int i = 0; i < 9; ++i)
        {
            result.m[i] = m[i] / v0;
        }
        return result;
    }

    void operator+=(const Matrix3x3& v0)
    {
        *this = *this + v0;
    }
    void operator-=(const Matrix3x3& v0)
    {
        *this = *this - v0;
    }
    void operator*=(const Matrix3x3& v0)
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

    // ベクトルは列ベクトル前提: result = M * v
    Vector3 operator*(const Vector3& v0) const
    {
        return Vector3 {
            m[0] * v0.x + m[3] * v0.y + m[6] * v0.z,
            m[1] * v0.x + m[4] * v0.y + m[7] * v0.z,
            m[2] * v0.x + m[5] * v0.y + m[8] * v0.z
        };
    }
};

} // namespace math
} // namespace slug
