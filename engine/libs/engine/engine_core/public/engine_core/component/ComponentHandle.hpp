#pragma once

#include <cstdint>

namespace slug::engine_core
{

class IComponent;
class ComponentManager;

class ComponentHandle
{
public:
    ComponentHandle() = default;
    ~ComponentHandle() = default;

    template<typename T>
    T* Get() const
    {
        return core::SObjectUtility::CheckedCast<T>(Get());
    }

    IComponent* Get() const;
    bool IsValid() const;
    explicit operator bool() const;

private:
    friend class ComponentManager;
    ComponentHandle(ComponentManager* manager, uint32_t index, uint32_t generation);

    ComponentManager* m_manager = nullptr;
    uint32_t m_index = UINT32_MAX;
    uint32_t m_generation = 0;
};
}
