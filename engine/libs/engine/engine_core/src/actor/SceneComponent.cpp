#include "engine_core/actor/SceneComponent.hpp"
#include "engine_core/actor/ActorComponentUtility.hpp"
#include "engine_core/actor/Actor.hpp"
#include "core/pattern/Closure.hpp"

namespace slug::engine_core
{

SLUG_COMPONENT_REGISTER(SceneComponent)

SceneComponent::SceneComponent()
    : m_parent(nullptr)
    , m_transform()
    , m_worldMatrix()
    , m_transformDirty(false)
    , m_rootFlag(false)
{
}

void SceneComponent::Initialize()
{
    SceneComponent::GetStaticComponentTypeId();
}

void SceneComponent::Start()
{

}

void SceneComponent::Tick(const engine_core::TickParam& tickParam)
{

}

void SceneComponent::End()
{

}

void SceneComponent::SetLocalPosition(const math::Vector3& value)
{
    m_transform.position = value;
    SetTransformDirty(true);
}

void SceneComponent::SetLocalRotation(const math::Vector3& value)
{
    m_transform.rotation = value;
    SetTransformDirty(true);
}

void SceneComponent::SetLocalScale(const math::Vector3& value)
{
    m_transform.scale = value;
    SetTransformDirty(true);
}

void SceneComponent::SetLocalTransform(const math::Transform& value)
{
    m_transform = value;
    SetTransformDirty(true);
}


void SceneComponent::SetWorldPosition(const math::Vector3& value)
{
    const math::Matrix4x4 parent = GetParentWorldMatrix();
    m_transform.SetWorldPosition(parent, value);
    SetTransformDirty(true);
}

void SceneComponent::SetWorldRotation(const math::Vector3& value)
{
    const math::Matrix4x4 parent = GetParentWorldMatrix();
    m_transform.SetWorldRotation(parent, value);
    SetTransformDirty(true);
}

void SceneComponent::SetWorldScale(const math::Vector3& value)
{
    const math::Matrix4x4 parent = GetParentWorldMatrix();
    m_transform.SetWorldScale(parent, value);
    SetTransformDirty(true);
}

void SceneComponent::SetWorldTransform(const math::Transform& value)
{
    SetWorldPosition(value.position);
    SetWorldRotation(value.rotation);
    SetWorldScale(value.scale);
}

void SceneComponent::SetTransformDirty(bool flag)
{
    m_transformDirty = true;
    for (auto& child : m_chidren)
    {
        child->SetTransformDirty(flag);
    }
}

math::Vector3 SceneComponent::GetLocalPosition() const
{
    return m_transform.position;
}

math::Vector3 SceneComponent::GetLocalRotation() const
{
    return m_transform.rotation;
}

math::Vector3 SceneComponent::GetLocalScale() const
{
    return m_transform.scale;
}

const math::Transform SceneComponent::GetLocalTransform() const
{
    return m_transform;
}

math::Vector3 SceneComponent::GetWorldPosition()
{
    const math::Matrix4x4 parent = GetParentWorldMatrix();
    return m_transform.GetWorldPosition(parent);
}

math::Vector3 SceneComponent::GetWorldRotation()
{
    const math::Matrix4x4 parent = GetParentWorldMatrix();
    return m_transform.GetWorldRotation(parent);
}

math::Vector3 SceneComponent::GetWorldScale()
{
    const math::Matrix4x4 parent = GetParentWorldMatrix();
    return m_transform.GetWorldScale(parent);
}

const math::Transform SceneComponent::GetWorldTransform()
{
    math::Transform ret = {};
    ret.position = GetWorldPosition();
    ret.rotation = GetWorldRotation();
    ret.scale = GetWorldScale();
    return ret;
}

bool SceneComponent::GetTransformDirty() const
{
    return m_transformDirty;
}

math::Matrix4x4 SceneComponent::GetWorldMatrix()
{
    if (m_transformDirty)
    {
        UpdateWorldMatrix();
    }
    return m_worldMatrix;
}

math::Matrix4x4 SceneComponent::GetPreWorldMatrix()
{
    return m_preWorldMatrix;
}

math::Matrix4x4 SceneComponent::GetParentWorldMatrix() 
{
    if (m_attachParam.type == AttachType::Relative)
    {
        if (m_parent != nullptr)
        {
            return m_parent->GetWorldMatrix();
        }
    }
    return math::Matrix4x4::Identity();
}

void SceneComponent::UpdateWorldMatrix()
{
    m_preWorldMatrix = m_worldMatrix;
    m_worldMatrix = GetParentWorldMatrix() * m_transform.GetMatrix();
    m_transformDirty = false;
}

void SceneComponent::SetRootFlag(bool flag)
{
    m_rootFlag = flag;
}

bool SceneComponent::CheckRoot()
{
    return m_rootFlag;
}

void SceneComponent::AttachToParent(SceneComponent* parent, const AttachParam& param)
{
    if (m_parent != nullptr)
    {
        m_parent->DeleteChild(this->GetUUID());
    }

    if (parent != nullptr)
    {
        parent->AttachToChild(this, param);
    }

    m_parent = parent;
    m_attachParam = param;
}

SceneComponent* SceneComponent::GetParent()
{
    return m_parent;
}

void SceneComponent::AttachToChild(SceneComponent* child, const AttachParam& param)
{
    if (!HasChild(child->GetUUID()))
    {
        m_chidren.push_back(child);
        child->AttachToParent(this, param);
    }
}

bool SceneComponent::HasChild(core::UUID uuid) const
{
    for (auto& child : m_chidren)
    {
        if (child != nullptr && child->GetUUID() == uuid)
        {
            return true;
        }
    }
    return false;
}

void SceneComponent::DeleteChild(core::UUID uuid)
{
    auto itr = m_chidren.begin();
    while (itr != m_chidren.end())
    {
        if ((*itr)->GetUUID() == uuid)
        {
            itr = m_chidren.erase(itr);
        }
        else
        {
            itr++;
        }
    }
}

SceneComponent* SceneComponent::GetChild(core::UUID uuid)
{
    for (auto& child : m_chidren)
    {
        if (child != nullptr && child->GetUUID() == uuid)
        {
            return child;
        }
    }
    return nullptr;
}

core::TVector<SceneComponentPtr>& SceneComponent::GetChildren()
{
    return m_chidren;
}

}
