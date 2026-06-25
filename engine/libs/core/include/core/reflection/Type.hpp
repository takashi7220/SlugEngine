#pragma once

#include "core/reflection/Constructor.hpp"
#include "core/reflection/Property.hpp"
#include "core/reflection/Method.hpp"
#include "core/reflection/Attribute.hpp"
#include "core/container/String.hpp"
#include "core/container/Vector.hpp"
#include "core/memory/ReferencePtr.hpp"
#include "core/TypeConcept.hpp"

namespace slug::core
{
class Property;
class Method;
class Constructor;

class Type
{
public:
    Type() = default;

    Type(TypeId id, core::StringView name, const Type* base = nullptr)
        : m_id(id)
        , m_name(name)
        , m_base(base)
    {
    }

    TypeId GetId() const
    {
        return m_id;
    }

    const core::String& GetName() const
    {
        return m_name;
    }

    const Type* GetBaseType() const
    {
        return m_base;
    }

    void SetBaseType(const Type* base)
    {
        m_base = base;
    }

    bool IsDerivedFrom(const Type* other) const
    {
        if (!other)
        {
            return false;
        }

        const Type* cur = this;
        while (cur)
        {
            if (cur == other)
            {
                return true;
            }
            cur = cur->m_base;
        }
        return false;
    }

    const core::TVector<core::TReferencePtr<Property>>& GetProperties() const
    {
        return m_properties;
    }

    const core::TVector<core::TReferencePtr<Method>>& GetMethods() const
    {
        return m_methods;
    }

    const core::TVector<core::TReferencePtr<Constructor>>& GetConstructors() const
    {
        return m_constructors;
    }
    void AddProperty(core::TReferencePtr<Property>& property);
    void AddMethod(core::TReferencePtr<Method>& method);
    void AddConstructor(core::TReferencePtr<Constructor>& ctor);

    const core::TReferencePtr<Property> FindProperty(core::StringView name) const;
    const core::TReferencePtr<Method> FindMethod(core::StringView name) const;

private:
    TypeId m_id = 0;
    core::String m_name;
    const Type* m_base = nullptr;
    core::TVector<core::TReferencePtr<Property>> m_properties;
    core::TVector<core::TReferencePtr<Method>> m_methods;
    core::TVector<core::TReferencePtr<Constructor>> m_constructors;
};
}
