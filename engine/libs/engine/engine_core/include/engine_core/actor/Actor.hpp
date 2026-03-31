#pragma once
#include "core/container/List.hpp"
#include "engine_core/tick/TickObject.hpp"
#include "engine_core/actor/ActorComponent.hpp"
#include "engine_core/actor/SceneComponent.hpp"
#include "engine_core/actor/ActorUtility.hpp"
#include "engine_core/actor/SpawnParameter.hpp"

#define SLUG_ACTOR_REGISTER(type)                                                                       \
namespace                                                                                               \
{                                                                                                       \
struct AutoRegisterGenerator_##type                                                                     \
{                                                                                                       \
    AutoRegisterGenerator_##type()                                                                      \
    {                                                                                                   \
        slug::engine_core::ActorUtility::RegisterGenerator(&type::GetStaticActorTypeId, &type::Create);   \
    }                                                                                                   \
} autoRegisterGenerator_##type;                                                                           \
}                                                                                                       \

#define SLUG_ACTOR_MEMBER(type)                                                                         \
static engine_core::ActorTypeID GetStaticActorTypeId()                                                  \
{                                                                                                       \
    return engine_core::ActorTypeID(#type);                                                             \
}                                                                                                       \
static void GatherAncestorTypes(core::TVector<engine_core::ActorTypeID>& types)                         \
{                                                                                                       \
                                                                                                        \
}                                                                                                       \
static core::TVector<engine_core::ActorTypeID> GetStaticAncestorActorTypeIds()                          \
{                                                                                                       \
    core::TVector<engine_core::ActorTypeID> ret;                                                        \
    return ret;                                                                                         \
}                                                                                                       \
static Actor* Create(const engine_core::ActorTypeID& actorTypeID)                                       \
{                                                                                                       \
    type* actor(new type());                                                                            \
    actor->SetActorTypeID(type::GetStaticActorTypeId());                                                \
    actor->SetAncestorActorTypeIDs(type::GetStaticAncestorActorTypeIds());                              \
    actor->SetName(type::GetStaticActorTypeId().GetName());                                             \
    actor->Initialize();                                                                                \
    return core::MemoryUtilities::CheckedCast<engine_core::Actor*, type*>(actor);                       \
}                                                                                                       \

#define SLUG_DERIVED_ACTOR_MEMBER(type, baseType)                                                       \
static engine_core::ActorTypeID GetStaticActorTypeId()                                                  \
{                                                                                                       \
    return engine_core::ActorTypeID(#type);                                                             \
}                                                                                                       \
static void GatherAncestorTypes(core::TVector<engine_core::ActorTypeID>& types)                         \
{                                                                                                       \
    types.push_back(baseType::GetStaticActorTypeId());                                                  \
    baseType::GatherAncestorTypes(types);                                                               \
}                                                                                                       \
static core::TVector<engine_core::ActorTypeID> GetStaticAncestorActorTypeIds()                          \
{                                                                                                       \
    core::TVector<engine_core::ActorTypeID> ret;                                                        \
    type::GatherAncestorTypes(ret);                                                                     \
    return ret;                                                                                         \
}                                                                                                       \
static Actor* Create(const engine_core::ActorTypeID& actorTypeID)                                       \
{                                                                                                       \
    type* actor(new type());                                                                            \
    actor->SetActorTypeID(type::GetStaticActorTypeId());                                                \
    actor->SetAncestorActorTypeIDs(type::GetStaticAncestorActorTypeIds());                              \
    actor->SetName(type::GetStaticActorTypeId().GetName());                                             \
    actor->Initialize();                                                                                \
    return core::MemoryUtilities::CheckedCast<engine_core::Actor*, type*>(actor);                       \
}                                                                                                       \


namespace slug::engine_core
{

class Actor;
using ActorPtr = core::TReferencePtr<Actor>;

class Level;
class IWorld;

class Actor : public TickObject, public core::SObject, public core::ReflectionClass
{
public:
    Actor();

    virtual void Initialize() = 0;
    virtual void Start() = 0;
    virtual void Tick(const TickParam& tickParam) override;
    virtual void End() = 0;

    // parent actor
    void SetParentActor(Actor* parent);
    Actor* GetParentActor();

    // world
    IWorld* GetWorld();
    void SetWorld(IWorld* world);

    // level
    Level* GetLevel();
    void SetLevel(Level* level);

    // child
    void AddChild(Actor* child);
    void DeleteChild(core::UUID uuid);
    bool HasChild(core::UUID uuid);
    Actor* GetChild(core::UUID uuid);
    void GetChildren(core::TVector<Actor*>& children, bool recursive  = false);

    // root component
    void SetRootComponent(SceneComponent* component);
    SceneComponent* GetRootComponent();

    // actor component
    ActorComponent* AddActorComponent(ComponentTypeID type);
    void AddActorComponent(ActorComponent* component);
    void DeleteActorComponent(core::UUID uuid);
    bool HasActorComponent(core::UUID uuid);
    void GetActorComponents(ComponentTypeID type, core::TVector<ActorComponent*>& components);
    ActorComponent* GetActorComponent(core::UUID uuid);

    // actor type id
    void SetActorTypeID(const ActorTypeID type);
    ActorTypeID GetActorTypeID();

    // ancestor actor type ids
    void SetAncestorActorTypeIDs(const core::TVector<ActorTypeID>& ids);
    const core::TVector<ActorTypeID>& GetAncestorActorTypeIDs();

public:
    template<ActorComponentType T>
    ActorComponentPtr AddComponent()
    {
        return AddComponent(T::GetStaticComponentTypeId());
    }

    template<ActorComponentType T>
    void DeleteComponent()
    {
        DeleteComponent(T::GetStaticComponentTypeId());
    }

    template<ActorComponentType T>
    bool HasComponent()
    {
        return HasComponent(T::GetStaticComponentTypeId());
    }

    template<ActorComponentType T>
    void GetComponents(core::TVector<ActorComponent*>& components)
    {
        GetComponents(T::GetStaticComponentTypeId(), components);
    }

private:
    IWorld* m_world;
    Level* m_level;
    Actor* m_parent;
    SceneComponentPtr m_rootComponent;
    core::TList<ActorPtr> m_children;
    core::TList<ActorComponentPtr> m_actorComponents;
    ActorTypeID m_actorTypeId;
    core::TVector<ActorTypeID> m_ancestorActorTypeIds;
};

}
