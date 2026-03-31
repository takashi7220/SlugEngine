#include "engine_core/actor/Actor.hpp"
#include "engine_core/actor/SceneComponent.hpp"
#include "engine_core/level/Level.hpp"
#include "engine_core/world/IWorld.hpp"

namespace slug::engine_core
{
Actor::Actor()
{
}

void Actor::Tick(const TickParam& tickParam)
{

}

void Actor::SetParentActor(Actor* parent)
{
    if (m_parent != nullptr)
    {
        m_parent->DeleteChild(this->GetUUID());
        m_parent->GetRootComponent()->DeleteChild(this->GetRootComponent()->GetUUID());
    }

    if (parent != nullptr)
    {
        parent->AddChild(this);
        parent->GetRootComponent()->AttachToChild(this->GetRootComponent(), SceneComponent::AttachParam::Default());
    }
    m_parent = parent;
}

Actor* Actor::GetParentActor()
{
    return m_parent;
}

Level* Actor::GetLevel()
{
    return m_level;
}

void Actor::SetLevel(Level* level)
{
    if (m_level != nullptr)
    {
        m_level->DeleteActor(this->GetUUID());
    }

    if (level != nullptr)
    {
        level->AddActor(this);
    }
    m_level = level;
}

IWorld* Actor::GetWorld()
{
    return m_world;
}

void Actor::SetWorld(IWorld* world)
{
    m_world = world;
}


void Actor::AddChild(Actor* child)
{
    if (!HasChild(child->GetUUID()))
    {
        m_children.push_back(child);
        child->SetParentActor(this);
    }
}

void Actor::DeleteChild(core::UUID uuid)
{
    auto itr = m_children.begin();
    while (itr != m_children.end())
    {
        if ((*itr)->GetUUID() == uuid)
        {
            itr = m_children.erase(itr);
        }
        else
        {
            itr++;
        }
    }
}

bool Actor::HasChild(core::UUID uuid)
{
    for (auto& child : m_children)
    {
        if (child->GetUUID() == uuid)
        {
            return true;
        }
    }
    return false;
}

Actor* Actor::GetChild(core::UUID uuid)
{
    for (auto& child : m_children)
    {
        if (child->GetUUID() == uuid)
        {
            return child;
        }
    }
    return nullptr;
}

void Actor::GetChildren(core::TVector<Actor*>& children, bool recursive)
{
    for (auto& child : m_children)
    {
        children.push_back(child);
        if (recursive)
        {
            child->GetChildren(children);
        }
    }

    
}

void Actor::SetRootComponent(SceneComponent* component)
{
    if (m_rootComponent != nullptr)
    {
        m_rootComponent->SetRootFlag(false);
        m_rootComponent->SetActor(nullptr);
    }

    if (component != nullptr)
    {
        component->SetRootFlag(true);
        component->SetActor(this);
    }

    m_rootComponent = component;
}

SceneComponent* Actor::GetRootComponent()
{
    if (m_rootComponent == nullptr)
    {
        ActorComponent* component = ActorComponentUtility::Generate(SceneComponent::GetStaticComponentTypeId());
        component->SetName("Root");
        SetRootComponent(core::MemoryUtilities::CheckedCast<SceneComponent*>(component));
    }
    return m_rootComponent;
}

ActorComponent* Actor::AddActorComponent(ComponentTypeID type)
{
    ActorComponent* component = ActorComponentUtility::Generate(type);
    component->Initialize();
    AddActorComponent(component);
    return component;
}

void Actor::AddActorComponent(ActorComponent* component)
{
    if (!HasActorComponent(component->GetUUID()))
    {
        m_actorComponents.push_back(component);
        component->SetActor(this);
    }
}

void Actor::DeleteActorComponent(core::UUID uuid)
{
    auto itr = m_actorComponents.begin();
    while (itr != m_actorComponents.end())
    {
        if (itr->get()->GetUUID() == uuid)
        {
            itr = m_actorComponents.erase(itr);
        }
        else
        {
            itr++;
        }
    }
}

bool Actor::HasActorComponent(core::UUID uuid)
{
    for (auto& component : m_actorComponents)
    {
        if (component->GetUUID() == uuid)
        {
            return true;
        }
    }

    return false;
}

void Actor::GetActorComponents(ComponentTypeID type, core::TVector<ActorComponent*>& components)
{
    for (auto& component : m_actorComponents)
    {
        if (component->GetComponentTypeID() == type)
        {
            components.push_back(component);
        }
    }
}

ActorComponent* Actor::GetActorComponent(core::UUID uuid)
{
    for (auto& tmp : m_actorComponents)
    {
        if (tmp->GetUUID() == uuid)
        {
            return tmp;
        }
    }

    return nullptr;
}


void Actor::SetActorTypeID(const ActorTypeID type)
{
    m_actorTypeId = type;
}

ActorTypeID Actor::GetActorTypeID()
{
    return m_actorTypeId;
}

void Actor::SetAncestorActorTypeIDs(const core::TVector<ActorTypeID>& ids)
{
    m_ancestorActorTypeIds = ids;
}

const core::TVector<ActorTypeID>& Actor::GetAncestorActorTypeIDs()
{
    return m_ancestorActorTypeIds;
}

}
