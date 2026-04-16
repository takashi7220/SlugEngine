#pragma once

#include "core/container/String.hpp"
#include "core/container/UnorderedMap.hpp"
#include "core/utility/PropertyChanged.hpp"
#include "core/memory/MemoryUtilities.hpp"
#include "core/debug/Assert.hpp"
#include "core/reflection/Reflection.hpp"

namespace slug::core
{
class ReflectionClass : public IPorpertyChanged
{
public:
    ReflectionClass();
    virtual void OnPropertyChanged(const PropertyChangedEventArgs& args);

public:
    template<typename T>
    bool SetValue(core::StringView propertyName, const T& data)
    {
#if 0
        Instance instance = Instance(this);
        Type type = instance.get_derived_type();
        Property property = type.get_property(propertyName.data());

        if (!property.is_valid() || property.is_readonly())
        {
            return false;
        }

        Type dstValueType = property.get_type();
        Type srcValueType = Type::get<T>();

        if (dstValueType != srcValueType)
        {
            return false;
        }

        return property.set_value(instance, data);
#endif
        return false;
    }

    template<typename T>
    bool GetValue(core::StringView propertyName, T& data)
    {
#if 0
        Instance instance = Instance(this);
        Type type = instance.get_derived_type();
        Property property = type.get_property(propertyName.data());

        if (!property.is_valid())
        {
            return false;
        }

        Type srcValueType = property.get_type();
        Type dstValueType = Type::get<T>();

        if (dstValueType != srcValueType)
        {
            return false;
        }

        Variant value = property.get_value(instance);
        if (!value.is_valid())
        {
            return false;
        }
        data = value.get_value<T>();
        return true;
#else
        return false;
#endif
    }
};
}
