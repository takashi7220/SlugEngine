#include "core/reflection/ReflectionUtility.hpp"
#include "core/reflection/ReflectionService.hpp"

namespace slug::core
{
void ReflectionUtility::RegisterReflectionFactory(Node& node)
{
    GetRegisterReflectionFactories().push_back(&node);
}

ReflectionUtility::LinkedList& ReflectionUtility::GetRegisterReflectionFactories()
{
    static ReflectionUtility::LinkedList registeredFactory;
    return registeredFactory;
}

void ReflectionUtility::RegisterAllReflection(ReflectionService& reflectionService)
{
    auto& registeredFactory = GetRegisterReflectionFactories();
    registeredFactory.for_each([&reflectionService](Factory * factory)
    {
        factory->factory(reflectionService.GetRegistry());
    });
}
}
