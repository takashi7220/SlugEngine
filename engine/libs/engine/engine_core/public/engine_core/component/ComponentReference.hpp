#pragma once

#include "engine_core/component/ComponentHandle.hpp"
#include "engine_core/component/ComponentInfo.hpp"
#include "engine_core/component/IComponent.hpp"

namespace slug::engine_core
{

template<typename T>
class ComponentReference
{
public:
    ComponentReference(ComponentID componentId)
        : m_componentId(componentId)
    {
    };

    void Resolve(World* world)
    {

    }

    IComponent* Get()
    {
        return m_handle.Get();
    }

    T* Get()
    {
        return m_handle.Get<T>();
    }

private:
    ComponentID m_componentId;
    ComponentHandle* m_handle;
};
}
