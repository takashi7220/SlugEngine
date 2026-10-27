#pragma once

#include "engine_core/component/IComponent.hpp"

namespace slug::engine_core
{

class Actor;

/**
 * @brief Entity Component System Component
 */
class EcsComponent : public IComponent
{
public:
    SLUG_DECLARE_SOBJECT_TYPEINFO(EcsComponent, IComponent);

    EcsComponent(Actor* owner, ComponentInfo info, uint64_t entityId)
        : IComponent(owner, info, ComponentKind::Ecs)
        , m_entityId(entityId)
    {
    }

    SLUG_COPY_DEFAULT(EcsComponent);

    virtual ~EcsComponent() = default;

    virtual void Initialize(const IProperty* property) override = 0;
    virtual void UpdateProperties(const IProperty* property) override = 0;
    virtual void Terminate() override = 0;

private:
    uint64_t m_entityId;
};

}
