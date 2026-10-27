#pragma once

#include "core/object/SObject.hpp"
#include "engine_core/property/IProperty.hpp"
#include "engine_core/component/ComponentInfo.hpp"
#include "engine_core/component/ComponentHandle.hpp"

#define SLUG_DECLARE_COMPONENT(className)                                       \
    static const slug::engine_core::ComponentType& GetStaticComponentType() {   \
        return className;                                                       \
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
    IComponent(Actor* owner, ComponentInfo info, ComponentKind kind)
        : m_owner(owner)
        , m_info(info)
        , m_kind(kind)
    {
    }

    virtual ~IComponent() = default;
    SLUG_COPY_DEFAULT(IComponent);

    virtual void Initialize(const IProperty* property) = 0;
    virtual void UpdateProperties(const IProperty* property) = 0;
    virtual void Terminate() = 0;
    virtual void Resolve(World* world) = 0;

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

private:
    friend class ComponentManager;

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
