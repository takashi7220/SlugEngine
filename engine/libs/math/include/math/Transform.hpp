#pragma once

#include "math/Vector3.hpp"
#include "math/Quaternion.hpp"
#include "math/Matrix4x4.hpp"

namespace slug
{
namespace math
{

struct Transform
{
    Vector3 position;

    Vector3 rotation;

    Vector3 scale;

    void SetWorldPosition(const Matrix4x4& parent, const math::Vector3& value)
    {
        const Matrix4x4 inv = parent.Inverse();
        const Vector4 wp(value);
        position = (inv * wp).xyz();
    }

    void SetWorldRotation(const Matrix4x4& parent, const math::Vector3& value)
    {
        const Quaternion worldQ(value);
        const Quaternion parentQ = Quaternion::FromMatrix(parent);
        const Quaternion localQ = parentQ.Inverse() * worldQ;
        rotation = localQ.GetEulerXYZ();
    }

    void SetWorldScale(const Matrix4x4& parent, const math::Vector3& value)
    {
        Vector3 parentScale = parent.GetExtraScale();
        scale = Vector3(value/ parentScale);
    }

    void SetWorldQuaternion(const Matrix4x4& parent, const math::Quaternion& value)
    {
        const Quaternion parentQ = Quaternion::FromMatrix(parent);
        const Quaternion localQ = parentQ.Inverse() * value;
        rotation = localQ.GetEulerXYZ();
    }

    void SetQuaternion(const Quaternion& value)
    {
        rotation = value.GetEulerXYZ();
    }

    math::Vector3 GetWorldPosition(const Matrix4x4& parent) const
    {
        const Matrix4x4 world = parent * GetMatrix();
        return world.GetTranslate();
    }

    math::Vector3 GetWorldRotation(const Matrix4x4& parent) const
    {
        return GetWorldQuaternion(parent).GetEulerXYZ();
    }

    math::Vector3 GetWorldScale(const Matrix4x4& parent) const
    {
        Vector3 parentScale = parent.GetExtraScale();
        return parentScale * scale;
    }

    Quaternion GetWorldQuaternion(const Matrix4x4& parent) const
    {
        const Quaternion localQ(rotation);
        const Quaternion parentQ = Quaternion::FromMatrix(parent);
        const Quaternion worldQ = parentQ * localQ;
        return worldQ;
    }

    Quaternion GetQuaternion() const
    {
        return ToQuaternion(rotation);
    }

    Matrix4x4 GetMatrix() const
    {
        return ToTranslateMatrix(position) * ToRotationMatrix(rotation) * ToScaleMatrix(scale);
    }

    static Matrix4x4 ToTranslateMatrix(const Vector3& value)
    {
        Matrix4x4 ret = Matrix4x4::Identity();
        ret.at(3, 0) = value.x;
        ret.at(3, 1) = value.x;
        ret.at(3, 2) = value.x;
        return ret;
    }

    static Matrix4x4 ToRotationMatrix(const Vector3& value)
    {
        auto q = ToQuaternion(value);
        return q.GetMatrix4x4();
    }

    static Matrix4x4 ToScaleMatrix(const Vector3& value)
    {
        Matrix4x4 ret = Matrix4x4::Identity();
        ret.at(0, 0) = value.x;
        ret.at(1, 1) = value.x;
        ret.at(2, 2) = value.x;
        return ret;
    }

    static Quaternion ToQuaternion(const Vector3& value)
    {
        Quaternion ret = {};
        ret.SetEulerXYZ(value);
        return ret;
    }
};
}
}
