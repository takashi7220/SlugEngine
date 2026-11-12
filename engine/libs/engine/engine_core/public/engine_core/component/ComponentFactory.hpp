#pragma once

#include "engine_core/component/ComponentInfo.hpp"
#include "core/pattern/Factory.hpp"

namespace slug::engine_core
{
class Actor;
class IComponent;

struct ComponentDescriptor
{
    slug::engine_core::ComponentType key;
    IComponent* (*factory)(Actor*, const ComponentInfo&);
    ComponentDescriptor(slug::engine_core::ComponentType key, IComponent* (*factory)(Actor*, const ComponentInfo&))
        : key(key), factory(factory)
    {
    }
};

class ComponentFactory : public core::IFactory<ComponentDescriptor>
{
public:
    static IComponent* CreateComponent(Actor* actor, const ComponentInfo& componentInfo);
};

}
