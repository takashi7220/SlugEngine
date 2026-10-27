#include "engine_core/component/IComponent.hpp"
#include "engine_core/component/ComponentManager.hpp"
#include "core/debug/Assert.hpp"

namespace slug::engine_core
{

ComponentHandle ComponentManager::Register(IComponent* component)
{
    if (!component)
    {
        return {};
    }

    uint32_t index = 0;
    if (!m_freeIndices.empty())
    {
        index = m_freeIndices.back();
        m_freeIndices.pop_back();
    }
    else
    {
        if (m_slots.size() >= MaxComponents)
        {
            return {};
        }

        index = static_cast<uint32_t>(m_slots.size());
        m_slots.emplace_back();
    }

    Slot& slot = m_slots[index];
    SLUG_ASSERT(slot.component == nullptr);

    slot.component = component;
    ComponentHandle handle(this, index, slot.generation);
    component->SetComponentHandle(handle);
    return handle;
}

IComponent* ComponentManager::Resolve(uint32_t index, uint32_t generation) const
{
    return ResolveInternal(index, generation);
}

bool ComponentManager::Unregister(IComponent* component)
{
    if (component == nullptr)
    {
        return false;
    }
    return Unregister(component->GetComponentHandle());
}

bool ComponentManager::Unregister(const ComponentHandle& handle)
{
    if (handle.m_manager != this)
    {
        return false;
    }

    if (handle.m_index >= m_slots.size())
    {
        return false;
    }

    Slot& slot = m_slots[handle.m_index];
    if (!slot.component || slot.generation != handle.m_generation)
    {
        return false;
    }

    slot.component = nullptr;

    if (slot.generation != MaxComponents)
    {
        ++slot.generation;
        m_freeIndices.push_back(handle.m_index);
    }

    return true;
}

IComponent* ComponentManager::ResolveInternal(uint32_t index, uint32_t generation) const
{
    if (index >= m_slots.size())
    {
        return nullptr;
    }
    const Slot& slot = m_slots[index];
    if (slot.generation != generation)
    {
        return nullptr;
    }
    return slot.component;
}
}
