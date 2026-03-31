#include "engine_core/actor/ActorComponentUtility.hpp"
#include "core/container/UnorderedMap.hpp"
#include "core/debug/Assert.hpp"

namespace slug
{
namespace engine_core
{

static core::TUnorderedMap<ComponentTypeID, GenerateComponentPtr, ComponentTypeIDKeyHash, ComponentTypeIDKeyCompare> COMPONENT_GENERATOR_TABLE = {};

void ActorComponentUtility::RegisterGenerator(GetComponentTypeIDPtr getFunc, GenerateComponentPtr createFunc)
{
    ComponentTypeID componentTypeID = getFunc();
    if (COMPONENT_GENERATOR_TABLE.find(componentTypeID) == COMPONENT_GENERATOR_TABLE.end())
    {
        COMPONENT_GENERATOR_TABLE[componentTypeID] = createFunc;
    }
}

bool ActorComponentUtility::HasGenerator(const ComponentTypeID& componentTypeID)
{
    if (COMPONENT_GENERATOR_TABLE.find(componentTypeID) != COMPONENT_GENERATOR_TABLE.end())
    {
        return true;
    }
    return false;
}

ActorComponent* ActorComponentUtility::Generate(const ComponentTypeID& ComponentTypeID)
{
    if (COMPONENT_GENERATOR_TABLE.find(ComponentTypeID) != COMPONENT_GENERATOR_TABLE.end())
    {
        return COMPONENT_GENERATOR_TABLE[ComponentTypeID](ComponentTypeID);
    }

    SLUG_ASSERT_MSG(false, core::StringUtility::Sprintfs("Cant Find Component Generator [%s]",ComponentTypeID.id.c_str()).c_str());
    return nullptr;
}

}
}
