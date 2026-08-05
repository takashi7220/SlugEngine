#pragma once

#include "core/reflection/Variant.hpp"
#include "core/container/Vector.hpp"
#include "core/Function.hpp"

namespace slug::core
{

class Method;
using MethodPtr = core::TReferencePtr<Method>;

class Method : public core::ReferenceObject
{
public:
    using InvokeFn = core::TFunctionObject<Variant(Instance&, const core::TVector<Variant>&)>;
    using ConstInvokeFn = core::TFunctionObject<Variant(const Instance&, const core::TVector<Variant>&)>;

    Method() = default;

    Method(core::StringView name, const Type* ownerType, const Type* returnType, core::TVector<const Type*> paramTypes, bool isConst, InvokeFn invoke)
        : m_name(name)
        , m_ownerType(ownerType)
        , m_returnType(returnType)
        , m_paramTypes(std::move(paramTypes))
        , m_isConst(isConst)
        , m_invoke(std::move(invoke))
        , m_constInvoke(nullptr)
    {
    }


    Method(core::StringView name, const Type* ownerType, const Type* returnType, core::TVector<const Type*> paramTypes, bool isConst, ConstInvokeFn invoke)
        : m_name(name)
        , m_ownerType(ownerType)
        , m_returnType(returnType)
        , m_paramTypes(std::move(paramTypes))
        , m_isConst(isConst)
        , m_invoke(nullptr)
        , m_constInvoke(std::move(invoke))
    {
    }

    const core::String& GetName() const
    {
        return m_name;
    }

    const Type* GetOwnerType() const
    {
        return m_ownerType;
    }

    const Type* GetReturnType() const
    {
        return m_returnType;
    }

    const core::TVector<const Type*>& GetParamTypes() const
    {
        return m_paramTypes;
    }

    bool IsConst() const
    {
        return m_isConst;
    }

    Variant Invoke(const Instance& instance, const core::TVector<Variant>& args) const
    {
        if (!m_constInvoke)
        {
            return {};
        }
        return m_constInvoke(instance, args);
    }

    Variant Invoke(Instance& instance, const core::TVector<Variant>& args)
    {
        if (m_isConst)
        {
            if (!m_constInvoke)
            {
                return {};
            }
            return m_constInvoke(instance, args);
        }
        else
        {
            if (!m_invoke)
            {
                return {};
            }
            return m_invoke(instance, args);
        }
    }

private:
    core::String m_name;
    const Type* m_ownerType = nullptr;
    const Type* m_returnType = nullptr;
    core::TVector<const Type*> m_paramTypes;
    bool m_isConst = false;
    ConstInvokeFn m_constInvoke;
    InvokeFn m_invoke;
};
}
