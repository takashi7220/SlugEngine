#pragma once

#include "engine_core/actor/ActorComponent.hpp"
#include "engine_core/ClassReflection.hpp"
#include "math/Transform.hpp"

namespace slug::engine_core
{

class SceneComponent;
using SceneComponentPtr = core::TReferencePtr<SceneComponent>;

SLUG_REFLECTION(true)
class SceneComponent : public engine_core::ActorComponent
{
public:
    SLUG_COMPONENT_MEMBER(SceneComponent)

public:

    SLUG_REFLECTION(true)
    enum class AttachType : uint8_t
    {
        Absolute,
        Relative
    };

    SLUG_REFLECTION(true)
    struct AttachParam
    {
        core::FixedString<64> name = "";
        AttachType type = AttachType::Relative;

        static AttachParam Default()
        {
            AttachParam ret;
            ret.name = "Default";
            ret.type = AttachType::Relative;
            return ret;
        }
    };

public:
    SceneComponent();
    void Initialize() override;
    void Start() override;
    void Tick(const engine_core::TickParam& tickParam) override;
    void End() override;

    void SetLocalPosition(const math::Vector3& value);
    void SetLocalRotation(const math::Vector3& value);
    void SetLocalScale(const math::Vector3& value);
    void SetLocalTransform(const math::Transform& value);

    void SetWorldPosition(const math::Vector3& value);
    void SetWorldRotation(const math::Vector3& value);
    void SetWorldScale(const math::Vector3& value);
    void SetWorldTransform(const math::Transform& value);

    void SetTransformDirty(bool flag);

    math::Vector3 GetLocalPosition() const;
    math::Vector3 GetLocalRotation() const;
    math::Vector3 GetLocalScale() const;
    const math::Transform GetLocalTransform() const;

    math::Vector3 GetWorldPosition();
    math::Vector3 GetWorldRotation();
    math::Vector3 GetWorldScale();
    const math::Transform GetWorldTransform();
    bool GetTransformDirty() const;

    virtual math::Matrix4x4 GetWorldMatrix();
    math::Matrix4x4 GetPreWorldMatrix();
    math::Matrix4x4 GetParentWorldMatrix();

    void UpdateWorldMatrix();

public:
    void SetRootFlag(bool flag);
    bool CheckRoot();

    void AttachToParent(SceneComponent* parent, const AttachParam& param);
    SceneComponent* GetParent();

    void AttachToChild(SceneComponent* child, const AttachParam& param);
    bool HasChild(core::UUID uuid) const;
    void DeleteChild(core::UUID uuid);
    SceneComponent* GetChild(core::UUID uuid);
    core::TVector<SceneComponentPtr>& GetChildren();

private:
    SLUG_PROPERTY(PropertyEditType = EditAnywhere, Category = Default, GroupName = Default, DisplayName = Transform, Tooltip = Setting Transform, Getter = SceneComponent::GetLocalTransform, Setter = SceneComponent::SetLocalTransform)
    math::Transform m_transform;

    SLUG_PROPERTY(Ignore = true)
    SceneComponent* m_parent;

    SLUG_PROPERTY(Ignore = true)
    AttachParam m_attachParam;

    SLUG_PROPERTY(Ignore = true)
    core::TVector<SceneComponentPtr> m_chidren = {};

    SLUG_PROPERTY(Ignore = true)
    math::Matrix4x4 m_worldMatrix;

    SLUG_PROPERTY(Ignore = true)
    math::Matrix4x4 m_preWorldMatrix;

    SLUG_PROPERTY(Ignore = true)
    bool m_transformDirty;

    SLUG_PROPERTY(Ignore = true)
    bool m_rootFlag;
};

}
