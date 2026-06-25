#include "core/reflection/Type.hpp"

namespace slug::core
{

void Type::AddProperty(core::TReferencePtr<Property>& property)
{
    m_properties.push_back(property);
}

void Type::AddMethod(core::TReferencePtr<Method>& method)
{
    m_methods.push_back(method);
}

void Type::AddConstructor(core::TReferencePtr<Constructor>& ctor)
{
    m_constructors.push_back(ctor);
}

const core::TReferencePtr<Property> Type::FindProperty(core::StringView name) const
{
    for (const auto& property : m_properties)
    {
        if (property->GetName() == name)
        {
            return property;
        }
    }
    return nullptr;
}

const core::TReferencePtr<Method> Type::FindMethod(core::StringView name) const
{
    for (const auto& method : m_methods)
    {
        if (method->GetName() == name)
        {
            return method;
        }
    }
    return nullptr;
}


}
