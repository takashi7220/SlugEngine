#pragma once

#include "core/reflection/Registration.hpp"
#include "core/reflection/ReflectionUtility.hpp"
#include "core/service/ServiceProvider.hpp"

namespace slug::core
{

class ReflectionService : public IService
{
public:
    SLUG_SERVICE_REGISTER(ReflectionService)
    ReflectionService();
    ReflectionRegistry& GetRegistry();
    static ReflectionRegistry* GetStaticRegistry();
private:
    ReflectionRegistry m_registry;
    static ReflectionRegistry* m_staticRegistryPtr;
};
using ReflectionServicePtr = core::TReferencePtr<ReflectionService>;
}

#define SLUG_REGISTER_REFLECTION(T, Name)                                                                       \
    template<>                                                                                                  \
    struct slug::core::TypeResolver<T>                                                                          \
    {                                                                                                           \
        static const slug::core::Type& Get()                                                                    \
        {                                                                                                       \
            const slug::core::Type* t = slug::core::ReflectionService::GetStaticRegistry()->FindType(#Name);    \
            if (!t)                                                                                             \
            {                                                                                                   \
                SLUG_THROW_EXCEPTION("Type not registered: " #Name);                                            \
            }                                                                                                   \
            return *t;                                                                                          \
        }                                                                                                       \
    };                                                                                                          \
    static void RegisterReflectionInfo_##Name(slug::core::ReflectionRegistry& registry);                        \
    static slug::core::ReflectionUtility::Factory g_reflectionRegistry_##Name                                   \
    {                                                                                                           \
        [](slug::core::ReflectionRegistry& registry) -> void { RegisterReflectionInfo_##Name(registry);}        \
    };                                                                                                          \
    static slug::core::ReflectionUtility::Node g_reflectionRegistryNode_##Name                                  \
    {                                                                                                           \
        nullptr, nullptr, &g_reflectionRegistry_##Name                                                          \
    };                                                                                                          \
    static bool g_reflectionRegistryNode_registered_##Name =                                                    \
        (slug::core::ReflectionUtility::RegisterReflectionFactory(g_reflectionRegistryNode_##Name), true);      \
    static void RegisterReflectionInfo_##Name(slug::core::ReflectionRegistry& registry)                         \
