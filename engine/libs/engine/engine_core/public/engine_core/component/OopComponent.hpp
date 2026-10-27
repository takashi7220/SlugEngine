#pragma once

#include "engine_core/component/IComponent.hpp"
#include "engine_core/system/UpdateInfo.hpp"
#include "engine_core/system/StartInfo.hpp"
#include "engine_core/system/EventInfo.hpp"

namespace slug::engine_core
{

class Actor;

/**
 * @brief Object Oriented Programmng Component
 */
class OopComponent : public IComponent
{
public:
    SLUG_DECLARE_SOBJECT_TYPEINFO(OopComponent, IComponent);

    OopComponent(Actor* owner, ComponentInfo info)
        : IComponent(owner, info, ComponentKind::Oop)
    {}

    SLUG_COPY_DEFAULT(OopComponent);

    virtual ~OopComponent() = default;

    virtual void Initialize(const IProperty* property) override = 0;
    virtual void UpdateProperties(const IProperty* property) override = 0;
    virtual void Terminate() override = 0;

private:
    virtual void OnStart(const StartInfo& info) = 0;
    virtual void OnUpdate(const UpdateInfo& info) = 0;
    virtual void OnEvent(const EventInfo& info) = 0;
};

}
