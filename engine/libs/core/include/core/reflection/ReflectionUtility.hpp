#pragma once
#include "core/container/IntrusiveLinkedList.hpp"
#include "core/container/String.hpp"
#include "core/memory/ReferencePtr.hpp"

namespace slug::core
{

class ReflectionService;
class ReflectionRegistry;

class ReflectionUtility
{
public:
    struct Factory
    {
        void(*factory)(ReflectionRegistry&);
    };

    using LinkedList = core::IntrusiveLinkedList<Factory>;
    using Node = LinkedList::Node;

    static void RegisterReflectionFactory(Node& node);
    static LinkedList& GetRegisterReflectionFactories();
    static void RegisterAllReflection(ReflectionService& reflectionService);
};

}

// クラス定義の後、.cpp ファイル内で使用する
#define SLUG_AUTO_REGISTER_REFLECTION(reflectionClass)                                                                   \
    void RegisterReflectionInfo_##reflectionClass(slug::core::ReflectionRegistry& registry);                             \
    static slug::core::ReflectionUtility::Factory g_reflectionRegistry_##reflectionClass {                               \
        [](slug::core::ReflectionRegistry& registry) -> void { RegisterReflectionInfo_##reflectionClass(registry);}      \
    };                                                                                                                   \
    static slug::core::ReflectionUtility::Node g_reflectionRegistryNode_##reflectionClass {                              \
        nullptr, nullptr, &g_reflectionRegistry_##reflectionClass                                                        \
    };                                                                                                                   \
    static bool g_reflectionRegistryNode_##reflectionClass_registered =                                                  \
        (slug::core::ReflectionUtility::RegisterReflectionFactory(g_reflectionRegistryNode_##reflectionClass), true);    \
    static void RegisterReflectionInfo_##reflectionClass(slug::core::ReflectionRegistry& registry)                       \
