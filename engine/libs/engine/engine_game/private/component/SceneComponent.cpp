#include "engine_game/component/SceneComponent.hpp"
#include "engine_core/component/ComponentFactory.hpp"

namespace slug::engine_game
{

static slug::engine_core::ComponentFactory::CreatorRegistry g_creatorRegistory_SceneComponent {
    SceneComponent::GetStaticComponentType(),
    SceneComponent::Create
};

static slug::engine_core::ComponentFactory::Node g_creatorRegistoryNode_SceneComponent {
    nullptr, nullptr, &g_creatorRegistory_SceneComponent
};

static bool g_creatorRegistoryNode_SceneComponent_registered = (slug::engine_core::ComponentFactory::RegisterCreator(g_creatorRegistoryNode_SceneComponent), true);


SceneComponent::SceneComponent(engine_core::Actor* owner, const engine_core::ComponentInfo& info)
    : engine_core::EcsComponent(owner, info)
{
}

void SceneComponent::Initialize(const engine_core::IProperty* property)
{

}

void SceneComponent::UpdateProperties(const engine_core::IProperty* property)
{

}

void SceneComponent::Terminate()
{

}

}
