#include "engine_core/actor/EmptyActor.hpp"

namespace slug::engine_core
{

SLUG_ACTOR_REGISTER(EmptyActor)

void EmptyActor::Initialize()
{
    ActorComponent* component = ActorComponentUtility::Generate(SceneComponent::GetStaticComponentTypeId());
    SetRootComponent(core::MemoryUtilities::CheckedCast<SceneComponent*>(component));
}

void EmptyActor::Start()
{

}

void EmptyActor::Tick(const TickParam& tickParam)
{

}

void EmptyActor::End()
{

}
}
