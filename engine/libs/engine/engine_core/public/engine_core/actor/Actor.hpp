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



    2
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

    template<typename T>
    T* AddComponent(ComponentID componentId)
    {
        return core::SObjectUtility::CheckedCast<T>(AddComponent(T::GetStaticComponentType(), componentId));
    }

    IComponent* AddComponent(ComponentType type)
    {
        
    }

    IComponent* AddComponent(ComponentType type, ComponentID componentId)
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
    T* GetComponent(ComponentID componentId)
    {
        return core::SObjectUtility::CheckedCast<T>(GetComponent(componentId));
    }

    IComponent* GetComponent(ComponentID componentId)
    {
        auto itr = m_components.begin();
        while (itr != m_components.end())
        {
            ComponentPtr& component = (*itr);
            if (component != nullptr && component->GetComponentInfo().id == componentId)
            {
                return itr->get();
            }
            itr++;
        }
    }

    template<typename T>
    core::TVector<T*> GetComponents()
    {
        const auto components = GetComponents(T::GetStaticComponentType());
        core::TVector<T*> result;
        result.reserve(components.size());

        for (IComponent* component : components)
        {
            result.push_back(core::SObjectUtility::CheckedCast<T>(component));
        }
        return result;
    }

    core::TVector<IComponent*> GetComponents(ComponentType type)
    {
        core::TVector<IComponent*> components;
        for (const ComponentPtr& component : m_components)
        {
            if (component != nullptr && type == component->GetComponentInfo().type)
            {
                components.push_back(component.get());
            }
        }
        return components;
    }

    bool HasComponent(ComponentID componentId)
    {
        for (const auto& component : m_components)
        {
            if (component != nullptr && component->GetComponentInfo().id == componentId)
            {
                return true;
            }
        }
        return false;
    }

private:
    World* m_world;
    Level* m_ownerLevel;
    Actor* m_parent;
    core::TList<ComponentPtr> m_components;
};

}
