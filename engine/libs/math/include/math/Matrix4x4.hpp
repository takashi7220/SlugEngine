#pragma once

#include "math/MatrixBase.hpp"
#include "math/Vector4.hpp"

namespace slug
{
namespace math
{

struct Matrix4x4
{
    // 列優先（column-major）
    // m[col * 4 + row]
    ValueType m[16] = {};

    Matrix4x4()
    {
        for (size_t i = 0; i < 16; i++)
            m[i] = ValueType(0);
    }

    Matrix4x4(const float v[16])
    {
        for (size_t i = 0; i < 16; i++)
            m[i] = v[i];
    }

    // 行ごとの初期化子 → 列優先へ詰め替え
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
    {
        for (size_t i = 0; i < 16; ++i)
            m[i] = other.m[i];
    }

    // 列優先アクセス (row=i, col=j)
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
        return Matrix4x4 {};
    }

    static Matrix4x4 Identity()
    {
        return Matrix4x4 {
            {1,0,0,0},
            {0,1,0,0},
            {0,0,1,0},
            {0,0,0,1}
        };
    }

    Matrix4x4 Transpose() const
    {
        Matrix4x4 r;
        for (size_t col = 0; col < 4; ++col)
        {
            for (size_t row = 0; row < 4; ++row)
            {
                // r(col,row) = this(row,col)
                r.m[col * 4 + row] = m[row * 4 + col];
            }
        }
        return r;
    }

    Matrix4x4 Inverse() const
    {
        Matrix4x4 inv = Identity(); // I
        Matrix4x4 a = *this;      // A

        // 列優先のガウス・ジョルダン消去
        for (size_t i = 0; i < 4; i++)
        {
            ValueType diag = a.at(i, i);
            if (math::TEqual(diag, ValueType(0)))
                return Zero(); // 逆行列なし

            // ピボット行を正規化
            for (size_t col = 0; col < 4; ++col)
            {
                a.at(i, col) /= diag;
                inv.at(i, col) /= diag;
            }

            // 他の行を0に
            for (size_t row = 0; row < 4; ++row)
            {
                if (row != i)
                {
                    ValueType factor = a.at(row, i);
                    for (size_t col = 0; col < 4; ++col)
                    {
                        a.at(row, col) -= factor * a.at(i, col);
                        inv.at(row, col) -= factor * inv.at(i, col);
                    }
                }
            }
        }

        return inv;
    }

    Matrix4x4 Adjoint() const
    {
        Matrix4x4 ret = Matrix4x4::Identity();
        ret.m[0] = this->m[5] * this->m[10] - this->m[6] * this->m[9];
        ret.m[1] = this->m[6] * this->m[8] - this->m[4] * this->m[10];
        ret.m[2] = this->m[4] * this->m[9] - this->m[5] * this->m[8];

        ret.m[4] = this->m[9] * this->m[2] - this->m[10] * this->m[1];
        ret.m[5] = this->m[10] * this->m[0] - this->m[8] * this->m[2];
        ret.m[6] = this->m[8] * this->m[1] - this->m[9] * this->m[0];

        ret.m[8] = this->m[1] * this->m[6] - this->m[2] * this->m[5];
        ret.m[9] = this->m[2] * this->m[4] - this->m[0] * this->m[6];
        ret.m[10] = this->m[0] * this->m[5] - this->m[1] * this->m[4];

        return ret;
    }

    bool operator==(const Matrix4x4& v0) const
    {
        for (int i = 0; i < 16; ++i)
            if (m[i] != v0.m[i]) return false;
        return true;
    }

    Matrix4x4 operator+(const Matrix4x4& v0) const
    {
        Matrix4x4 r;
        for (int i = 0; i < 16; ++i)
            r.m[i] = m[i] + v0.m[i];
        return r;
    }

    Matrix4x4 operator-(const Matrix4x4& v0) const
    {
        Matrix4x4 r;
        for (int i = 0; i < 16; ++i)
            r.m[i] = m[i] - v0.m[i];
        return r;
    }

    // 列優先の行列積
    Matrix4x4 operator*(const Matrix4x4& v0) const
    {
        Matrix4x4 r;
        for (int col = 0; col < 4; ++col)
        {
            for (int row = 0; row < 4; ++row)
            {
                r.m[col * 4 + row] =
                    m[0 * 4 + row] * v0.m[col * 4 + 0] +
                    m[1 * 4 + row] * v0.m[col * 4 + 1] +
                    m[2 * 4 + row] * v0.m[col * 4 + 2] +
                    m[3 * 4 + row] * v0.m[col * 4 + 3];
            }
        }
        return r;
    }

    Matrix4x4 operator+(ValueType v0) const
    {
        Matrix4x4 r;
        for (int i = 0; i < 16; ++i)
            r.m[i] = m[i] + v0;
        return r;
    }

    Matrix4x4 operator-(ValueType v0) const
    {
        Matrix4x4 r;
        for (int i = 0; i < 16; ++i)
            r.m[i] = m[i] - v0;
        return r;
    }

    Matrix4x4 operator*(ValueType v0) const
    {
        Matrix4x4 r;
        for (int i = 0; i < 16; ++i)
            r.m[i] = m[i] * v0;
        return r;
    }

    Matrix4x4 operator/(ValueType v0) const
    {
        Matrix4x4 r;
        for (int i = 0; i < 16; ++i)
            r.m[i] = m[i] / v0;
        return r;
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

    // 列ベクトル前提
    Vector4 operator*(const Vector4& v0) const
    {
        return Vector4 {
            m[0] * v0.x + m[4] * v0.y + m[8] * v0.z + m[12] * v0.w,
            m[1] * v0.x + m[5] * v0.y + m[9] * v0.z + m[13] * v0.w,
            m[2] * v0.x + m[6] * v0.y + m[10] * v0.z + m[14] * v0.w,
            m[3] * v0.x + m[7] * v0.y + m[11] * v0.z + m[15] * v0.w
        };
    }

    Vector3 GetExtraScale() const
    {
        const float sx = math::TSqrt(m[0] * m[0] + m[1] * m[1] + m[2] * m[2]);
        const float sy = math::TSqrt(m[4] * m[4] + m[5] * m[5] + m[6] * m[6]);
        const float sz = math::TSqrt(m[8] * m[8] + m[9] * m[9] + m[10] * m[10]);
        return Vector3(sx, sy, sz );
    }

    Vector3 GetTranslate() const
    {
        return Vector3(m[3], m[7], m[11]);
    }

};

} // namespace math
} // namespace slug
