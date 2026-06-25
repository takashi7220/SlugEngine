#pragma once
#include "core/reflection/Property.hpp"
#include "core/reflection/Method.hpp"
#include "core/reflection/Constructor.hpp"
#include "core/reflection/ReflectionService.hpp"

namespace slug::core
{

template<typename T>
static T VariantCast(const Variant& v)
{
    return v.Get<remove_cvref_t<T>>();
}

template<typename R, typename C, typename... Args, std::size_t... I>
static Variant InvokeMemberImpl(C* obj, R(C::* method)(Args...), const core::TVector<Variant>& args, std::index_sequence<I...>)
{
    if constexpr (std::is_void_v<R>)
    {
        (obj->*method)(VariantCast<Args>(args[I])...);
        return {};
    }
    else
    {
        return Variant((obj->*method)(VariantCast<Args>(args[I])...));
    }
}

template<typename R, typename C, typename... Args, std::size_t... I>
static Variant InvokeMemberImpl(const C* obj, R(C::* method)(Args...) const, const core::TVector<Variant>& args, std::index_sequence<I...>)
{
    if constexpr (std::is_void_v<R>)
    {
        (obj->*method)(VariantCast<Args>(args[I])...);
        return {};
    }
    else
    {
        return Variant((obj->*method)(VariantCast<Args>(args[I])...));
    }
}

template<typename T, typename... Args, std::size_t... I>
static Variant InvokeCtorImpl(const core::TVector<Variant>& args, std::index_sequence<I...>)
{
    return Variant(T(VariantCast<Args>(args[I])...));
}

template<typename T>
class TypeRegister
{
public:
    explicit TypeRegister(Type& type)
        : m_type(type)
    {
    }

    template<typename BaseT>
    TypeRegister& base()
    {
        m_type.SetBaseType(&TypeOf<BaseT>());
        return *this;
    }

    template<typename... Args>
    TypeRegister& constructor()
    {
        core::TVector<const Type*> params = { ResolveTypePtr<Args>()... };

        ConstructorPtr ctor = core::MakeReference<Constructor>(
            &m_type,
            std::move(params),
            [](const core::TVector<Variant>& args) -> Variant {
                if (args.size() != sizeof...(Args))
                {
                    return {};
                }
                return InvokeCtorImpl<T, Args...>(args, std::index_sequence_for<Args...>{});
            });

        m_type.AddConstructor(ctor);
        return *this;
    }

    template<typename MemberT>
    TypeRegister& property(const char* name, MemberT T::* member, const PropertyDescriptor& descriptor = {})
    {
        PropertyPtr p = core::MakeReference<Property>(
            name,
            &m_type,
            &TypeOf<MemberT>(),
            true,
            true,
            [member](const Instance& inst) -> Variant {
                const T* obj = inst.TryAs<T>();
                if (!obj)
                {
                    return {};
                }
                return Variant(obj->*member);
            },
            [member](Instance& inst, const Variant& value) -> bool
            {
                if (inst.IsConst())
                {
                    return false;
                }

                T* obj = inst.TryAs<T>();
                if (!obj || !value.Is<MemberT>())
                {
                    return false;
                }

                obj->*member = value.Get<MemberT>();
                return true;
            },
            descriptor);

        m_type.AddProperty(p);
        return *this;
    }

    template<typename Getter, typename Setter>
    TypeRegister& property(const char* name, Getter getter, Setter setter, const PropertyDescriptor& descriptor = {})
    {
        using GetterTraits = GetterTraitsImpl<Getter>;
        using SetterTraits = SetterTraitsImpl<Setter>;
        using ValueT = typename GetterTraits::ReturnType;
        using SetterArgT = typename SetterTraits::ArgType;

        static_assert(std::is_same_v<remove_cvref_t<ValueT>, remove_cvref_t<SetterArgT>>,
                      "Getter return type and setter arg type must match.");

        PropertyPtr p = core::MakeReference<Property>(
            name,
            &m_type,
            &TypeOf<remove_cvref_t<ValueT>>(),
            true,
            true,
            [getter](const Instance& inst) -> Variant
            {
                const T* obj = inst.TryAs<T>();
                if (!obj)
                {
                    return {};
                }
                return Variant((obj->*getter)());
            },
            [setter](Instance& inst, const Variant& value) -> bool
            {
                using RawT = remove_cvref_t<SetterArgT>;

                if (inst.IsConst())
                {
                    return false;
                }

                T* obj = inst.TryAs<T>();
                if (!obj || !value.Is<RawT>())
                {
                    return false;
                }

                (obj->*setter)(value.Get<RawT>());
                return true;
            },
            descriptor);

        m_type.AddProperty(p);
        return *this;
    }

    template<typename Getter>
    TypeRegister& property_readonly(const char* name, Getter getter, const PropertyDescriptor& descriptor = {})
    {
        using GetterTraits = GetterTraitsImpl<Getter>;
        using ValueT = typename GetterTraits::ReturnType;

        PropertyPtr p = core::MakeReference<Property>(
            name,
            &m_type,
            &TypeOf<remove_cvref_t<ValueT>>(),
            true,
            false,
            [getter](const Instance& inst) -> Variant
            {
                const T* obj = inst.TryAs<T>();
                if (!obj)
                {
                    return {};
                }
                return Variant((obj->*getter)());
            },
            nullptr,
            descriptor);

        m_type.AddProperty(p);
        return *this;
    }

    template<typename R, typename... Args>
    TypeRegister& method(const char* name, R(T::* methodPtr)(Args...))
    {
        core::TVector<const Type*> params = { ResolveTypePtr<Args>()... };

        MethodPtr m = core::MakeReference<Method>(
            name,
            &m_type,
            ResolveReturnType<R>(),
            std::move(params),
            false,
            Method::InvokeFn([methodPtr](Instance& inst, const core::TVector<Variant>& args) -> Variant
            {
                if (args.size() != sizeof...(Args))
                {
                    return {};
                }

                T* obj = inst.TryAs<T>();
                if (!obj)
                {
                    return {};
                }

                return InvokeMemberImpl(obj, methodPtr, args, std::index_sequence_for<Args...>{});
            }));

        m_type.AddMethod(m);
        return *this;
    }

    template<typename R, typename... Args>
    TypeRegister& method(const char* name, R(T::* methodPtr)(Args...) const)
    {
        core::TVector<const Type*> params = { ResolveTypePtr<Args>()... };

        MethodPtr m = core::MakeReference<Method>(
            name,
            &m_type,
            ResolveReturnType<R>(),
            std::move(params),
            true,
            Method::ConstInvokeFn([methodPtr](const Instance& inst, const core::TVector<Variant>& args) -> Variant
            {
                if (args.size() != sizeof...(Args))
                {
                    return {};
                }

                const T* obj = inst.TryAs<T>();
                if (!obj)
                {
                    return {};
                }

                return InvokeMemberImpl(obj, methodPtr, args, std::index_sequence_for<Args...>{});
            }));

        m_type.AddMethod(m);
        return *this;
    }

private:
    template<typename G>
    struct GetterTraitsImpl;

    template<typename R>
    struct GetterTraitsImpl<R(T::*)() const>
    {
        using ReturnType = R;
    };

    template<typename S>
    struct SetterTraitsImpl;

    template<typename A>
    struct SetterTraitsImpl<void(T::*)(A)>
    {
        using ArgType = A;
    };

    template<typename R>
    static const Type* ResolveReturnType()
    {
        if constexpr (std::is_void_v<R>)
        {
            return nullptr;
        }
        else
        {
            return &TypeOf<R>();
        }
    }

    Type& m_type;
};

}
