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
    SLUG_COPY_DEFAULT(EcsComponent);

    virtual ~EcsComponent() = default;

    uint64_t GetEntityId()
    {
        return m_info.entityId;
    }

protected:
    EcsComponent(Actor* owner, const engine_core::ComponentInfo& info)
        : IComponent(owner, info, ComponentKind::Ecs)
    {
    }
private:
    virtual void Initialize(const IProperty* property) override = 0;
    virtual void UpdateProperties(const IProperty* property) override = 0;
    virtual void Terminate() override = 0;
};

}
