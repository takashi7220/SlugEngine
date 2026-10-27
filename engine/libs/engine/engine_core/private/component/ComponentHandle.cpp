#include "engine_core/component/IComponent.hpp"
#include "engine_core/component/ComponentManager.hpp"
#include "engine_core/component/ComponentHandle.hpp"

namespace slug::engine_core
{

ComponentHandle::ComponentHandle(ComponentManager* manager, uint32_t index, uint32_t generation)
    : m_manager(manager)
    , m_index(index)
    , m_generation(generation)
{
}

IComponent* ComponentHandle::Get() const
{
    if (m_manager == nullptr)
    {
        return nullptr;
    }
    return m_manager->Resolve(m_index, m_generation);
}

bool ComponentHandle::IsValid() const
{
    return Get() != nullptr;
}

explicit ComponentHandle::operator bool() const
{
    return IsValid();
}

}
