#pragma once

#include "engine_core/tick/TickObject.hpp"
#include "engine_core/actor/ActorComponentUtility.hpp"
#include "core/reflection/ReflectionClass.hpp"
#include "core/memory/MemoryUtilities.hpp"
#include "core/container/List.hpp"

#define SLUG_COMPONENT_REGISTER(type)                                                                               \
namespace                                                                                                           \
{                                                                                                                   \
struct AutoRegisterGenerator_##type                                                                                   \
{                                                                                                                   \
    AutoRegisterGenerator_##type()                                                                                    \
    {                                                                                                               \
        slug::engine_core::ActorComponentUtility::RegisterGenerator(&type::GetStaticComponentTypeId, &type::Create);  \
    }                                                                                                               \
} autoRegisterGenerator_##type;                                                                                       \
}                                                                                                                   \

#define SLUG_COMPONENT_MEMBER(type)                                                                                 \
static engine_core::ComponentTypeID GetStaticComponentTypeId()                                                      \
{                                                                                                                   \
    return engine_core::ComponentTypeID(#type);                                                                     \
}                                                                                                                   \
static void GatherAncestorTypes(core::TVector<engine_core::ComponentTypeID>& types)                                 \
{                                                                                                                   \
                                                                                                                    \
}                                                                                                                   \
static core::TVector<engine_core::ComponentTypeID> GetStaticAncestorComponentTypeIds()                              \
{                                                                                                                   \
    core::TVector<engine_core::ComponentTypeID> ret;                                                                \
    return ret;                                                                                                     \
}                                                                                                                   \
static ActorComponent* Create(const engine_core::ComponentTypeID& componentTypeID)                                  \
{                                                                                                                   \
    type* component(new type());                                                                                    \
    component->SetComponentTypeID(type::GetStaticComponentTypeId());                                                \
    component->SetAncestorComponentTypeIDs(type::GetStaticAncestorComponentTypeIds());                              \
    component->SetName(type::GetStaticComponentTypeId().GetName());                                                 \
    component->Initialize();                                                                                        \
    return core::MemoryUtilities::CheckedCast<engine_core::ActorComponent*, type*>(component);                      \
}                                                                                                                   \

#define SLUG_DERIVED_COMPONENT_MEMBER(type, baseType)                                                               \
static engine_core::ComponentTypeID GetStaticComponentTypeId()                                                      \
{                                                                                                                   \
    return engine_core::ComponentTypeID(#type);                                                                     \
}                                                                                                                   \
static void GatherAncestorTypes(core::TVector<engine_core::ComponentTypeID>& types)                                 \
{                                                                                                                   \
    types.push_back(baseType::GetStaticComponentTypeId());                                                          \
    baseType::GatherAncestorTypes(types);                                                                           \
}                                                                                                                   \
static core::TVector<engine_core::ComponentTypeID> GetStaticAncestorComponentTypeIds()                              \
{                                                                                                                   \
    core::TVector<engine_core::ComponentTypeID> ret;                                                                \
    type::GatherAncestorTypes(ret);                                                                                 \
    return ret;                                                                                                     \
}                                                                                                                   \
static ActorComponent* Create(const engine_core::ComponentTypeID& componentTypeID)                                  \
{                                                                                                                   \
    type* component(new type());                                                                                    \
    component->SetComponentTypeID(type::GetStaticComponentTypeId());                                                \
    component->SetAncestorComponentTypeIDs(type::GetStaticAncestorComponentTypeIds());                              \
    component->SetName(type::GetStaticComponentTypeId().GetName());                                                 \
    component->Initialize();                                                                                        \
    return core::MemoryUtilities::CheckedCast<engine_core::ActorComponent*, type*>(component);                      \
}                                                                                                                   \

namespace slug
{
namespace engine_core
{

class Actor;
class ActorComponent;
using ActorComponentPtr = core::TReferencePtr<ActorComponent>;

template<typename T>
concept ActorComponentType = std::derived_from<T, ActorComponent>;

class ActorComponent : public TickObject, public core::SObject, public core::ReflectionClass
{
public:
    ActorComponent();
    virtual void Initialize() = 0;
    virtual void Start() = 0;
    virtual void Tick(const TickParam& tickParam) override;
    virtual void End() = 0;

public:

    void SetComponentTypeID(const ComponentTypeID type);
    ComponentTypeID GetComponentTypeID() const;

    void SetAncestorComponentTypeIDs(const core::TVector<ComponentTypeID>& ids);
    const core::TVector<ComponentTypeID>& GetAncestorComponentTypeIDs();

    void SetActor(Actor* actor);
    Actor* GetActor();

protected:
    ComponentTypeID m_componentTypeId = {};
    core::TVector<ComponentTypeID> m_ancestorComponentTypeIds = {};
    Actor* m_actor = nullptr;
};

}
}
