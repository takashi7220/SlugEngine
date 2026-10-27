#pragma once

#include "engine_core/component/IComponent.hpp"
#include "core/object/SObject.hpp"
#include "core/container/List.hpp"

namespace slug::engine_core
{

class Actor : public core::SObject
{
public:
    SLUG_DECLARE_SOBJECT_TYPEINFO(Actor, core::SObject);

    Actor(World* world, Level* owner)
        : m_world(world)
        , m_ownerLevel(owner)
        , m_parent(nullptr)
        , m_components()
    {
    }

    virtual ~Actor() = default;
    SLUG_COPY_DEFAULT(Actor);

    virtual void Initialize(const IProperty* property) = 0;
    virtual void UpdateProperties(const IProperty* property) = 0;
    virtual void Terminate() = 0;
    virtual void Resolve() = 0;

    template<typename T>
    T* AddComponent()
    {
        return core::SObjectUtility::CheckedCast<T>(AddComponent(T::GetStaticComponentType()));
    }

    IComponent* AddComponent(ComponentType type)
    {

    }

    bool RemoveComponent(IComponent* component)
    {
        if (component != nullptr)
        {
            RemoveComponent(component->GetComponentInfo().id);
        }
        return false;
    }

    bool RemoveComponent(ComponentID componentId)
    {
        auto itr = m_components.begin();
        while (itr != m_components.end())
        {
            ComponentPtr& component = (*itr);
            if (component != nullptr && component->GetComponentInfo().id == componentId)
            {
                itr = m_components.erase(itr);
            }
            else
            {
                itr++;
            }
        }
    }

    template<typename T>
    T* GetComponent(ComponentID id);
    IComponent* GetComponent(ComponentID id);

    template<typename T>
    core::TVector<T*> GetComponents();
    core::TVector<IComponent*> GetComponents(ComponentType type);

    bool HasComponent(ComponentID componentId);

private:
    World* m_world;
    Level* m_ownerLevel;
    Actor* m_parent;
    core::TList<ComponentPtr> m_components;
};

}
