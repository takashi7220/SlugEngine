#pragma once

#include "core/reflection/TypeResolver.hpp"
#include "core/TypeConcept.hpp"

namespace slug::core
{
class Instance
{
public:
    Instance() = default;

    template<typename T, typename U = remove_cvref_t<T>, typename = std::enable_if_t<!std::is_same_v<U, Instance>>>
    explicit Instance(T& object)
        : m_mutablePtr(std::is_const_v<std::remove_reference_t<T>> ? nullptr : (void*)std::addressof(object))
        , m_constPtr((const void*)std::addressof(object))
        , m_typeId(TypeOf<U>().GetId())
        , m_isConst(std::is_const_v<std::remove_reference_t<T>>)
    {
    }

    static Instance FromMutable(void* ptr, TypeId typeId)
    {
        Instance i;
        i.m_mutablePtr = ptr;
        i.m_constPtr = ptr;
        i.m_typeId = typeId;
        i.m_isConst = false;
        return i;
    }

    static Instance FromConst(const void* ptr, TypeId typeId)
    {
        Instance i;
        i.m_mutablePtr = nullptr;
        i.m_constPtr = ptr;
        i.m_typeId = typeId;
        i.m_isConst = true;
        return i;
    }

    bool IsValid() const
    {
        return m_constPtr != nullptr;
    }

    bool IsConst() const
    {
        return m_isConst;
    }

    TypeId GetTypeId() const
    {
        return m_typeId;
    }

    void* MutableData() const
    {
        return m_mutablePtr;
    }

    const void* Data() const
    {
        return m_constPtr;
    }

    template<typename T>
    T* TryAs()
    {
        using U = remove_cvref_t<T>;
        if (m_isConst || !m_mutablePtr)
        {
            return nullptr;
        }
        if (m_typeId != TypeOf<U>().GetId())
        {
            return nullptr;
        }
        return static_cast<U*>(m_mutablePtr);
    }

    template<typename T>
    const T* TryAs() const
    {
        using U = remove_cvref_t<T>;
        if (!m_constPtr)
        {
            return nullptr;
        }
        if (m_typeId != TypeOf<U>().GetId())
        {
            return nullptr;
        }
        return static_cast<const U*>(m_constPtr);
    }

private:
    void* m_mutablePtr = nullptr;
    const void* m_constPtr = nullptr;
    TypeId m_typeId = 0;
    bool m_isConst = false;
};
}
