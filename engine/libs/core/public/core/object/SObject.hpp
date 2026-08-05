#pragma once

#include "core/Define.hpp"
#include "core/uuid/UUID.hpp"
#include "core/memory/MemoryUtility.hpp"
#include "math/Math.hpp"
#include "core/memory/ReferencePtr.hpp"
#include "core/object/SObjectType.hpp"

#define SLUG_DECLARE_SOBJECT_TYPEINFO(Class, BaseClass)                             \
    static constexpr slug::core::SObjectTypeID GetStaticTypeID()                    \
    {                                                                               \
        return slug::core::GetSObjectTypeID<Class>();                               \
    }                                                                               \
    virtual slug::core::SObjectTypeID GetDynamicTypeID() const override             \
    {                                                                               \
        return Class::GetStaticTypeID();                                            \
    }                                                                               \
    virtual bool IsAncestorType(slug::core::SObjectTypeID typeID) const override    \
    {                                                                               \
        return typeID == GetStaticTypeID() || BaseClass::IsAncestorType(typeID);    \
    }                                                                               \

namespace slug::core
{
class SObject : public core::ReferenceObject
{
public:
    static constexpr slug::core::SObjectTypeID GetStaticTypeID()
    {
        return slug::core::GetSObjectTypeID<SObject>();
    }

    virtual SObjectTypeID GetDynamicTypeID() const
    {
        return GetStaticTypeID();
    }

    virtual bool IsAncestorType(SObjectTypeID typeID) const
    {
        return typeID == GetStaticTypeID();
    }
};

class SObjectUtility
{
public:
public:
    template<typename To, typename From>
    static To* CheckedCast(From* u)
    {
        static_assert(!std::is_same_v<To, From>, "Redundant checked_cast");
        static_assert(std::is_base_of_v<slug::core::SObject, From>, "From must derive from Object");
        static_assert(std::is_base_of_v<slug::core::SObject, To>, "To must derive from Object");
#if SLUG_DEBUG_MODE
        if (!u->IsAncestorType(To::GetStaticTypeID()))
        {
            throw std::runtime_error("Invalid type cast");
        }
#endif
        return static_cast<To*>(u);
    }
};
}
