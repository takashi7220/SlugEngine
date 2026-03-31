#include "engine_core/actor/ActorComponent.hpp"
#include "engine_core/actor/Actor.hpp"


namespace slug
{
namespace engine_core
{

ActorComponent::ActorComponent()
{
}

void ActorComponent::Tick(const TickParam& tickParam)
{

}

void ActorComponent::SetComponentTypeID(const ComponentTypeID type)
{
    m_componentTypeId = type;
}

ComponentTypeID ActorComponent::GetComponentTypeID() const
{
    return m_componentTypeId;
}

void ActorComponent::SetAncestorComponentTypeIDs(const core::TVector<ComponentTypeID>& ids)
{
    m_ancestorComponentTypeIds = ids;
}

const core::TVector<ComponentTypeID>& ActorComponent::GetAncestorComponentTypeIDs()
{
    return m_ancestorComponentTypeIds;
}


void ActorComponent::SetActor(Actor* actor)
{
    m_actor = actor;
}

Actor* ActorComponent::GetActor()
{
    return m_actor;
}

}
}
