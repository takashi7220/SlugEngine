#pragma once

#include "engine_core/component/ComponentHandle.hpp"
#include "core/container/Vector.hpp"

namespace slug::engine_core
{

class IComponent;

class ComponentManager
{
public:
    static constexpr uint32_t MaxComponents = UINT32_MAX;

    ComponentHandle Register(IComponent* component);
    bool Unregister(IComponent* component);
    bool Unregister(const ComponentHandle& handle);

    IComponent* Resolve(uint32_t index, uint32_t generation) const;

private:
    struct Slot
    {
        IComponent* component = nullptr;
        uint32_t generation = 1;
    };

    IComponent* ResolveInternal(uint32_t index, uint32_t generation) const;

    core::TVector<Slot> m_slots;
    core::TVector<uint32_t> m_freeIndices;
};
}
