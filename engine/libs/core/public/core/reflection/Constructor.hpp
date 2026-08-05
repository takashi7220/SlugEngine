#pragma once

#include "core/reflection/Variant.hpp"
#include "core/Function.hpp"

namespace slug::core
{

class Constructor;
using ConstructorPtr = core::TReferencePtr<Constructor>;

class Constructor : public core::ReferenceObject
{
public:
    using InvokeFn = core::TFunctionObject<Variant(const core::TVector<Variant>&)>;

    Constructor() = default;

    Constructor(const Type* ownerType, core::TVector<const Type*> paramTypes, InvokeFn invoke)
        : m_ownerType(ownerType)
        , m_paramTypes(std::move(paramTypes))
        , m_invoke(std::move(invoke))
    {
    }

    const Type* GetOwnerType() const
    {
        return m_ownerType;
    }

    const core::TVector<const Type*>& GetParamTypes() const
    {
        return m_paramTypes;
    }

    Variant Invoke(const core::TVector<Variant>& args) const
    {
        if (!m_invoke)
        {
            return {};
        }
        return m_invoke(args);
    }

private:
    const Type* m_ownerType = nullptr;
    core::TVector<const Type*> m_paramTypes;
    InvokeFn m_invoke;
};
}
