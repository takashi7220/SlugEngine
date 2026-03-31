#pragma once

#include "math/TypeConcept.hpp"
#include "math/Vector3.hpp"
#include "math/Vector4.hpp"
#include "math/Matrix3x3.hpp"
#include "math/Matrix4x4.hpp"

namespace slug
{
namespace math
{

struct Quaternion
{
    ValueType x;
    ValueType y;
    ValueType z;
    ValueType w;

    Quaternion()
        : x(0)
        , y(0)
        , z(0)
        , w(1)
    {}

    Quaternion(ValueType x, ValueType y, ValueType z, ValueType w)
        : x(x)
        , y(y)
        , z(z)
        , w(w)
    {}

    Quaternion(const Vector3& eulerXYZ)
    {
        SetEulerXYZ(eulerXYZ);
    }

    Quaternion(std::initializer_list<float> list)
        : x(0)
        , y(0)
        , z(0)
        , w(1)
    {
        auto it = list.begin();
        if (list.size() >= 4)
        {
            x = *it++;
            y = *it++;
            z = *it++;
            w = *it++;
        }
    }

    Quaternion& operator=(const Vector4& v0)
    {
        x = v0.x;
        y = v0.y;
        z = v0.z;
        w = v0.w;
        return *this;
    }

    Quaternion operator*(const Quaternion& q) const
    {
        return Quaternion(
            w * q.x + x * q.w + y * q.z - z * q.y,
            w * q.y - x * q.z + y * q.w + z * q.x,
            w * q.z + x * q.y - y * q.x + z * q.w,
            w * q.w - x * q.x - y * q.y - z * q.z
        );
    }

    Vector3 operator*(const Vector3& v) const
    {
        const Vector3 qv(this->x, this->y, this->z);
        const Vector3 t = qv.Cross(v) * 2.0f;
        return v + t * this->w + qv.Cross(t);
    }

    void InitializeEulerX(ValueType angle, bool degree = true)
    {
        float radius = degree ? math::Deg2Rad(angle) : angle;
        ValueType half = radius * 0.5f;
        x = math::Sin(half);
        y = 0;
        z = 0;
        w = math::Cos(half);
    }

    void InitializeEulerY(ValueType angle, bool degree = true)
    {
        float radius = degree ? math::Deg2Rad(angle) : angle;
        ValueType half = radius * 0.5f;
        x = 0;
        y = math::Sin(half);
        z = 0;
        w = math::Cos(half);
    }

    void InitializeEulerZ(ValueType angle, bool degree = true)
    {
        float radius = degree ? math::Deg2Rad(angle) : angle;
        ValueType half = radius * 0.5f;
        x = 0;
        y = 0;
        z = math::Sin(half);
        w = math::Cos(half);
    }

    Vector4 GetVector4() const
    {
        return Vector4(x, y, z, w);
    }

    void SetEulerX(ValueType angle, bool degree = true)
    {
        Vector3 euler = GetEulerXYZ(degree);
        euler.x = angle;
        SetEulerXYZ(euler, degree);
    }

    void SetEulerY(ValueType angle, bool degree = true)
    {
        Vector3 euler = GetEulerXYZ(degree);
        euler.y = angle;
        SetEulerXYZ(euler, degree);
    }

    void SetEulerZ(ValueType angle, bool degree = true)
    {
        Vector3 euler = GetEulerXYZ(degree);
        euler.z = angle;
        SetEulerXYZ(euler, degree);
    }

    void SetEulerXYZ(const Vector3& euler, bool degree = true)
    {
        Quaternion qx, qy, qz;
        qx.InitializeEulerX(euler.x);
        qy.InitializeEulerY(euler.y);
        qz.InitializeEulerZ(euler.z);
        *this = qz * qy * qx;
    }

    void AddEulerX(ValueType angle, bool degree = true)
    {
        SetEulerX(angle + GetEulerX(degree), degree);
    }

    void AddEulerY(ValueType angle, bool degree = true)
    {
        SetEulerY(angle + GetEulerY(degree), degree);
    }

    void AddEulerZ(ValueType angle, bool degree = true)
    {
        SetEulerZ(angle + GetEulerZ(degree), degree);
    }

    void AddEulerXYZ(Vector3 delta, bool degree = true)
    {
        SetEulerXYZ(delta + GetEulerXYZ(degree), degree);
    }

    void SetRotation(const Vector3& axis, ValueType angle, bool degree = true)
    {
        ValueType radius = degree ? Deg2Rad(angle) : angle;
        ValueType half = radius * 0.5f;
        ValueType s = math::Sin(half);
        ValueType c = math::Cos(half);
        x = axis.x * s;
        y = axis.y * s;
        z = axis.z * s;
        w = c;
    }

    ValueType GetEulerX(bool degree = true) const
    {
        ValueType v0 = 2.0f * (w * x + y * z);
        ValueType v1 = 1.0f - 2.0f * (x * x + y * y);
        ValueType angle = std::atan2(v0, v1);
        return degree ? Rad2Deg(angle) : angle;
    }

    ValueType GetEulerY(bool degree = true) const
    {
        ValueType v0 = 2.0f * (w * y - z * x);
        ValueType angle = 0.0f;
        if (std::fabs(v0) >= 1.0f)
        {
            angle = std::copysign(3.1415926535f / 2.0f, v0);
        }
        angle = std::asin(v0);
        return degree ? Rad2Deg(angle) : angle;
    }

    ValueType GetEulerZ(bool degree = true) const
    {
        ValueType v0 = 2.0f * (w * z + x * y);
        ValueType v1 = 1.0f - 2.0f * (y * y + z * z);
        ValueType angle = std::atan2(v0, v1);
        return degree ? Rad2Deg(angle) : angle;
    }

    Vector3 GetEulerXYZ(bool degree = true) const
    {
        return Vector3(GetEulerX(degree), GetEulerY(degree), GetEulerZ(degree));
    }

    Matrix3x3 GetMatrix3x3() const
    {
        Matrix3x3 mat;
        mat.at(0, 0) = 1 - 2 * (y * y + z * z);
        mat.at(0, 1) = 2 * (x * y - z * w);
        mat.at(0, 2) = 2 * (x * z + y * w);

        mat.at(1, 0) = 2 * (x * y + z * w);
        mat.at(1, 1) = 1 - 2 * (x * x + z * z);
        mat.at(1, 2) = 2 * (y * z - x * w);

        mat.at(2, 0) = 2 * (x * z - y * w);
        mat.at(2, 1) = 2 * (y * z + x * w);
        mat.at(2, 2) = 1 - 2 * (x * x + y * y);
        return mat;
    }

    Matrix4x4 GetMatrix4x4() const
    {
        Matrix4x4 mat;

        mat.at(0, 0) = 1 - 2 * (y * y + z * z);
        mat.at(0, 1) = 2 * (x * y - z * w);
        mat.at(0, 2) = 2 * (x * z + y * w);
        mat.at(0, 3) = 0;

        mat.at(1, 0) = 2 * (x * y + z * w);
        mat.at(1, 1) = 1 - 2 * (x * x + z * z);
        mat.at(1, 2) = 2 * (y * z - x * w);
        mat.at(1, 3) = 0;

        mat.at(2, 0) = 2 * (x * z - y * w);
        mat.at(2, 1) = 2 * (y * z + x * w);
        mat.at(2, 2) = 1 - 2 * (x * x + y * y);
        mat.at(2, 3) = 0;

        mat.at(3, 0) = 0;
        mat.at(3, 1) = 0;
        mat.at(3, 2) = 0;
        mat.at(3, 3) = 1;

        return mat;
    }

    Quaternion Inverse() const
    {
        const float lenSq = x * x + y * y + z * z + w * w;

        if (lenSq == 0.0f)
        {
            return Quaternion(0, 0, 0, 1);
        }

        const float invLen = 1.0f / lenSq;
        return Quaternion(
            -x * invLen,
            -y * invLen,
            -z * invLen,
             w * invLen
        );
    }

    static Quaternion FromMatrix(const Matrix4x4& mat)
    {
        Quaternion q;
        float trace = mat.m[0] + mat.m[5] + mat.m[10]; // m00 + m11 + m22

        if (trace > 0.0f)
        {
            float s = std::sqrt(trace + 1.0f) * 2.0f;
            q.w = 0.25f * s;
            q.x = (mat.m[9]  - mat.m[6]) / s;   // (m21 - m12)
            q.y = (mat.m[2]  - mat.m[8]) / s;   // (m02 - m20)
            q.z = (mat.m[4]  - mat.m[1]) / s;   // (m10 - m01)
        }
        else if (mat.m[0] > mat.m[5] && mat.m[0] > mat.m[10])
        {
            float s = std::sqrt(1.0f + mat.m[0] - mat.m[5] - mat.m[10]) * 2.0f;
            q.w = (mat.m[9]  - mat.m[6]) / s;
            q.x = 0.25f * s;
            q.y = (mat.m[1] + mat.m[4]) / s;
            q.z = (mat.m[2] + mat.m[8]) / s;
        }
        else if (mat.m[5] > mat.m[10])
        {
            float s = std::sqrt(1.0f + mat.m[5] - mat.m[0] - mat.m[10]) * 2.0f;
            q.w = (mat.m[2] - mat.m[8]) / s;
            q.x = (mat.m[1] + mat.m[4]) / s;
            q.y = 0.25f * s;
            q.z = (mat.m[6] + mat.m[9]) / s;
        }
        else
        {
            float s = std::sqrt(1.0f + mat.m[10] - mat.m[0] - mat.m[5]) * 2.0f;
            q.w = (mat.m[4] - mat.m[1]) / s;
            q.x = (mat.m[2] + mat.m[8]) / s;
            q.y = (mat.m[6] + mat.m[9]) / s;
            q.z = 0.25f * s;
        }

        return q;
    }
};
}
}
