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
    SLUG_COPY_DEFAULT(OopComponent);

    virtual ~OopComponent() = default;
protected:
    OopComponent(Actor* owner, const engine_core::ComponentInfo& info)
        : IComponent(owner, info, ComponentKind::Oop)
    {
    }

private:
    virtual void Initialize(const IProperty* property) override = 0;
    virtual void UpdateProperties(const IProperty* property) override = 0;
    virtual void Terminate() override = 0;

    virtual void OnStart(const StartInfo& info) = 0;
    virtual void OnUpdate(const UpdateInfo& info) = 0;
    virtual void OnEvent(const EventInfo& info) = 0;
};

}
