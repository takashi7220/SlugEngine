#pragma once

#include "core/object/SObject.hpp"
#include "engine_core/property/IProperty.hpp"
#include "engine_core/component/ComponentInfo.hpp"
#include "engine_core/component/ComponentHandle.hpp"

#define SLUG_DECLARE_COMPONENT(className)                                       \
    static const slug::engine_core::ComponentType& GetStaticComponentType() {   \
        return slug::engine_core::ComponentType(#className);                    \
    }                                                                           \
    static slug::engine_core::IComponent* Create(                               \
        slug::engine_core::Actor* owner,                                        \
        const slug::engine_core::ComponentInfo& info) {                         \
        return slug::core::NewObject<className>(owner, info);                   \
    }                                                                           \

namespace slug::engine_core
{
class Actor;
class World;

enum class ComponentKind
{
    Oop,
    Ecs
};

class IComponent;
using ComponentPtr = core::TReferencePtr<IComponent>;

class IComponent : public core::SObject
{
public:
    SLUG_DECLARE_SOBJECT_TYPEINFO(IComponent, core::SObject);

    virtual ~IComponent() = default;
    SLUG_COPY_DEFAULT(IComponent);

    const ComponentInfo& GetComponentInfo() const
    {
        return m_info;
    }

    const ComponentKind& GetComponentKind() const
    {
        return m_kind;
    }

    const ComponentHandle& GetComponentHandle() const
    {
        return m_handle;
    }
protected:
    IComponent(Actor* owner, const engine_core::ComponentInfo& info, ComponentKind kind)
        : m_owner(owner)
        , m_info(info)
        , m_kind(kind)
    {
    }

    friend class ComponentManager;

    virtual void Initialize(const IProperty* property) = 0;
    virtual void UpdateProperties(const IProperty* property) = 0;
    virtual void Terminate() = 0;
    virtual void Resolve(World* world) = 0;

    void SetComponentHandle(const ComponentHandle& handle)
    {
        m_handle = handle;
    }

    ComponentInfo m_info;
    ComponentKind m_kind;
    ComponentHandle m_handle;
    Actor* m_owner;
};

}
