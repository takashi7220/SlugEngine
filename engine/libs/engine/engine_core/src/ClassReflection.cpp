#include "engine_core/ClassReflection.hpp"
#include "engine_core/actor/SceneComponent.hpp"

namespace slug::engine_core
{

static constexpr core::StringView SceneComponent_ReflectionMemberNames[] =
{
    "m_transform",
};

static const core::PropertyDescriptor SceneComponent_ReflectionMemberDescriptors[] =
{
    {core::PropertyEditType::EditAnywhere, "Default", "Transform", "Settings Transform", "Default" },

};

core::StringView SceneComponent_ReflectionClass::GetMemberName(MemberType type)
{
    uint32_t index = static_cast<uint32_t>(type);
    return SceneComponent_ReflectionMemberNames[index];
}

core::PropertyDescriptor SceneComponent_ReflectionClass::GetDescriptor(MemberType type)
{
    uint32_t index = static_cast<uint32_t>(type);
    return SceneComponent_ReflectionMemberDescriptors[index];
}


SLUG_REGISTRATION
{
Registration::class_<SceneComponent>("SceneComponent")
    .constructor<>()
    .property("m_transform", &SceneComponent::GetLocalTransform, &SceneComponent::SetLocalTransform);
}

}
