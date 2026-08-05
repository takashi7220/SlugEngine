#pragma once

#include "core/debug/Exception.hpp"
#include "core/TypeConcept.hpp"
#include "core/reflection/Attribute.hpp"

namespace slug::core
{

class Type;

template<typename T>
struct TypeResolver
{
    static const Type& Get()
    {
        static_assert(sizeof(T) == 0, "TypeResolver<T> is not specialized for this type.");
        SLUG_THROW_EXCEPTION("Unreachable");
    }
};

template<typename T>
inline const Type& TypeOf()
{
    return TypeResolver<remove_cvref_t<T>>::Get();
}

template<typename T>
static const Type* ResolveTypePtr()
{
    return &TypeOf<T>();
}

}

