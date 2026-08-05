#pragma once

#include "core/reflection/Attribute.hpp"
#include "core/reflection/Instance.hpp"
#include "core/reflection/Variant.hpp"
#include "core/container/String.hpp"
#include "core/Function.hpp"

namespace slug::core
{

class Property;
using PropertyPtr = core::TReferencePtr<Property>;

class Property : public core::ReferenceObject
{
public:
    using GetterFn = core::TFunctionObject<Variant(const Instance&)>;
    using SetterFn = core::TFunctionObject<bool(Instance&, const Variant&)>;

    Property() = default;

    Property(core::StringView name, const Type* ownerType, const Type* valueType, bool readable, bool writable, GetterFn getter, SetterFn setter, const PropertyDescriptor& descriptor)
        : m_name(std::move(name))
        , m_ownerType(ownerType)
        , m_valueType(valueType)
        , m_readable(readable)
        , m_writable(writable)
        , m_getter(std::move(getter))
        , m_setter(std::move(setter))
        , m_descriptor(descriptor)
    {
    }

    const core::StringView GetName() const
    {
        return m_name;
    }

    const Type* GetOwnerType() const
    {
        return m_ownerType;
    }

    const Type* GetValueType() const
    {
        return m_valueType;
    }

    bool IsReadable() const
    {
        return m_readable;
    }

    bool IsWritable() const
    {
        return m_writable;
    }

    Variant GetValue(const Instance& instance) const
    {
        if (!m_readable || !m_getter)
        {
            return {};
        }
        return m_getter(instance);
    }

    bool SetValue(Instance& instance, const Variant& value) const
    {
        if (!m_writable || !m_setter)
        {
            return false;
        }
        return m_setter(instance, value);
    }

private:
    core::String m_name;
    const Type* m_ownerType = nullptr;
    const Type* m_valueType = nullptr;
    bool m_readable = false;
    bool m_writable = false;
    GetterFn m_getter;
    SetterFn m_setter;
    PropertyDescriptor m_descriptor;
};
}
