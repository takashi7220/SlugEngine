#pragma once

#include "math/Math.hpp"
#include "math/Vector2.hpp"

namespace slug::math
{

struct Matrix2x2
{
    // 列優先（column-major）: m[col * 2 + row]
    // [ a b ] -> m = { a, c, b, d }
    // [ c d ]
    ValueType m[4] = {};

    Matrix2x2()
    {
        for (size_t i = 0; i < 4; i++)
        {
            this->m[i] = ValueType(0);
        }
    }

    // 行ごとの初期化子を列優先に詰め替える
    Matrix2x2(std::initializer_list<std::initializer_list<ValueType>> list)
    {
        for (size_t i = 0; i < 4; ++i)
        {
            m[i] = ValueType(0);
        }

        auto rowList = list.begin();
        for (size_t row = 0; row < 2 && rowList != list.end(); ++row, ++rowList)
        {
            auto colList = rowList->begin();
            for (size_t col = 0; col < 2 && colList != rowList->end(); ++col, ++colList)
            {
                m[col * 2 + row] = *colList; // 列優先
            }
        }
    }

    Matrix2x2(const Matrix2x2& m0)
    {
        for (size_t i = 0; i < 4; i++)
        {
            this->m[i] = m0.m[i];
        }
    }

    // 列優先アクセス: (row, col) -> m[col*2 + row]
    ValueType& at(size_t row, size_t col)
    {
        return m[col * 2 + row];
    }
    const ValueType& at(size_t row, size_t col) const
    {
        return m[col * 2 + row];
    }

    static Matrix2x2 Zero()
    {
        return Matrix2x2 {};
    }

    static Matrix2x2 Identity()
    {
        // 行ごとに記述してOK（ctorが列優先へ格納）
        return Matrix2x2 {
            {ValueType(1), ValueType(0)},
            {ValueType(0), ValueType(1)}
        };
    }

    Matrix2x2 Transpose() const
    {
        Matrix2x2 r;
        // r(col,row) = this(row,col)
        r.m[0] = m[0]; // (0,0)
        r.m[3] = m[3]; // (1,1)
        r.m[1] = m[2]; // (1,0) <- (0,1)
        r.m[2] = m[1]; // (0,1) <- (1,0)
        return r;
    }

    Matrix2x2 Inverse() const
    {
        // a b
        // c d
        const ValueType a = m[0];
        const ValueType c = m[1];
        const ValueType b = m[2];
        const ValueType d = m[3];

        const ValueType det = a * d - b * c;
        if (det == ValueType(0))
        {
            return Zero();
        }

        const ValueType invDet = ValueType(1) / det;

        // 行で書いているが、ctorが列優先に詰め替える
        return Matrix2x2 {
            { d * invDet, -b * invDet },
            { -c * invDet,  a * invDet }
        };
    }

    bool operator==(const Matrix2x2& v0) const
    {
        for (int i = 0; i < 4; ++i)
            if (m[i] != v0.m[i]) return false;
        return true;
    }

    Matrix2x2 operator+(const Matrix2x2& v0) const
    {
        Matrix2x2 result;
        for (int i = 0; i < 4; ++i) result.m[i] = m[i] + v0.m[i];
        return result;
    }

    Matrix2x2 operator-(const Matrix2x2& v0) const
    {
        Matrix2x2 result;
        for (int i = 0; i < 4; ++i) result.m[i] = m[i] - v0.m[i];
        return result;
    }

    // 列優先想定の行列積: r = (*this) * v0
    Matrix2x2 operator*(const Matrix2x2& v0) const
    {
        Matrix2x2 r;
        for (int col = 0; col < 2; ++col)
        {
            for (int row = 0; row < 2; ++row)
            {
                r.m[col * 2 + row] =
                    m[0 * 2 + row] * v0.m[col * 2 + 0] +
                    m[1 * 2 + row] * v0.m[col * 2 + 1];
            }
        }
        return r;
    }

    Matrix2x2 operator+(ValueType v0) const
    {
        Matrix2x2 result;
        for (int i = 0; i < 4; ++i) result.m[i] = m[i] + v0;
        return result;
    }

    Matrix2x2 operator-(ValueType v0) const
    {
        Matrix2x2 result;
        for (int i = 0; i < 4; ++i) result.m[i] = m[i] - v0;
        return result;
    }

    Matrix2x2 operator*(ValueType v0) const
    {
        Matrix2x2 result;
        for (int i = 0; i < 4; ++i) result.m[i] = m[i] * v0;
        return result;
    }

    Matrix2x2 operator/(ValueType v0) const
    {
        Matrix2x2 result;
        for (int i = 0; i < 4; ++i) result.m[i] = m[i] / v0;
        return result;
    }

    void operator+=(const Matrix2x2& v0)
    {
        *this = *this + v0;
    }
    void operator-=(const Matrix2x2& v0)
    {
        *this = *this - v0;
    }
    void operator*=(const Matrix2x2& v0)
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

    // 列ベクトル前提: result = M * v
    Vector2 operator*(const Vector2& v0) const
    {
        return Vector2 {
            m[0] * v0.x + m[2] * v0.y, // r0 = a*x + b*y
            m[1] * v0.x + m[3] * v0.y  // r1 = c*x + d*y
        };
    }
};

} // namespace math
} // namespace slug
