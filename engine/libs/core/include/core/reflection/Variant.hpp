#pragma once

#include "core/reflection/TypeResolver.hpp"
#include "core/memory/ReferencePtr.hpp"
#include "core/TypeConcept.hpp"


namespace slug::core
{

class Variant
{
public:
    Variant() = default;

    Variant(const Variant&) = default;
    Variant(Variant&&) noexcept = default;
    Variant& operator=(const Variant&) = default;
    Variant& operator=(Variant&&) noexcept = default;

    template<typename T, typename U = remove_cvref_t<T>, typename = std::enable_if_t<!std::is_same_v<U, Variant>>>
    Variant(T&& value)
        : m_ptr(core::MakeReference<Model<U>>(std::forward<T>(value)))
        , m_typeId(TypeOf<U>().GetId())
    {
    }

    bool IsValid() const
    {
        return m_ptr != nullptr;
    }

    void Reset()
    {
        m_typeId = 0;
        m_ptr.reset();
    }

    TypeId GetTypeId() const
    {
        return m_typeId;
    }

    template<typename T>
    bool Is() const
    {
        return m_ptr && m_typeId == TypeOf<remove_cvref_t<T>>().GetId();
    }

    template<typename T>
    const T& Get() const
    {
        return static_cast<const Model<remove_cvref_t<T>>*>(m_ptr.get())->value;
    }

    template<typename T>
    T& Get()
    {
        return static_cast<Model<remove_cvref_t<T>>*>(m_ptr.get())->value;
    }

private:
    struct Concept : core::ReferenceObject
    {
        virtual ~Concept() = default;
    };

    template<typename T>
    struct Model final : Concept
    {
        template<typename U>
        explicit Model(U&& v)
            : value(std::forward<U>(v))
        {
        }
        T value;
    };
private:
    TypeId m_typeId = 0;
    core::TReferencePtr<Concept> m_ptr;
};
}
