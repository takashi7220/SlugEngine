#include "core/reflection/Registration.hpp"
#include "core/reflection/ReflectionService.hpp"
#include "core/reflection/ReflectionUtility.hpp"
namespace slug::core
{

ReflectionRegistry* ReflectionService::m_staticRegistryPtr = nullptr;

ReflectionService::ReflectionService()
{
    m_staticRegistryPtr = &m_registry;
    ReflectionUtility::RegisterAllReflection(*this);
}

ReflectionRegistry& ReflectionService::GetRegistry()
{
    return m_registry;
}

ReflectionRegistry* ReflectionService::GetStaticRegistry()
{
    if (m_staticRegistryPtr == nullptr)
    {
        SLUG_THROW_EXCEPTION("Unsupporeted Reflection Service. ");
    }
    return m_staticRegistryPtr;
}

}
