#pragma once

#include "engine_core/component/EcsComponent.hpp"
#include "math/Transform.hpp"

namespace slug::engine_game
{
class SceneComponent : engine_core::EcsComponent
{
public:
    SLUG_DECLARE_SOBJECT_TYPEINFO(SceneComponent, EcsComponent);
    SLUG_DECLARE_COMPONENT(slug::engine_game::SceneComponent)

protected:
    SceneComponent(engine_core::Actor* owner, const engine_core::ComponentInfo& info);

private:
    virtual void Initialize(const engine_core::IProperty* property) override;
    virtual void UpdateProperties(const engine_core::IProperty* property) override;
    virtual void Terminate() override;

};
}
