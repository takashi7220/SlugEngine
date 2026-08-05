#pragma one

#include "core/container/String.hpp"
#include "math/Matrix4x4.hpp"
#include "math/Transform.hpp"
#include "sgal/geometry/Attribute.hpp"

namespace slug::algorithm
{

struct Node;
using NodePtr = core::TReferencePtr<Node>;

struct Node : core::ReferenceObject
{
    core::FixedString<128> name;
    math::Transform localTransform;
    core::TVector<AttributePtr> attributes;
    Node* parent = nullptr;
    core::TVector<NodePtr> children;

    math::Transform GetWorldTransform() const
    {
        if (parent)
        {
            return parent->GetWorldTransform() * localTransform;
        }
        else
        {
            return localTransform;
        }
    }

    math::Matrix4x4 GetWorldMatrix() const
    {
        return GetWorldTransform().GetMatrix();
    }

    math::Matrix4x4 GetLocalMatrix() const
    {
        return localTransform.GetMatrix();
    }

    math::Matrix4x4 GetBindPoseMatrix() const
    {
        return GetWorldMatrix().Inverse();
    }

    math::Matrix4x4 GetInverseBindPoseMatrix() const
    {
        return GetWorldMatrix();
    }
};
}
