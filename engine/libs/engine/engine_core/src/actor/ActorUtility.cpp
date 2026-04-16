#include "engine_core/actor/ActorUtility.hpp"
#include "core/container/UnorderedMap.hpp"
#include "core/debug/Assert.hpp"


namespace slug::engine_core
{
static core::TUnorderedMap<ActorTypeID, GenerateActorPtr, ActorTypeIDKeyHash, ActorTypeIDKeyCompare> ACTOR_GENERATOR_TABLE = {};

void ActorUtility::RegisterGenerator(GetActorTypeIDPtr getFunc, GenerateActorPtr createFunc)
{
    ActorTypeID actorTypeID = getFunc();
    if (ACTOR_GENERATOR_TABLE.find(actorTypeID) == ACTOR_GENERATOR_TABLE.end())
    {
        ACTOR_GENERATOR_TABLE[actorTypeID] = createFunc;
    }
}

bool ActorUtility::HasGenerator(const ActorTypeID& actorTypeID)
{
    if (ACTOR_GENERATOR_TABLE.find(actorTypeID) != ACTOR_GENERATOR_TABLE.end())
    {
        return true;
    }
    return false;
}

Actor* ActorUtility::Generate(const ActorTypeID& ActorTypeID)
{
    if (ACTOR_GENERATOR_TABLE.find(ActorTypeID) != ACTOR_GENERATOR_TABLE.end())
    {
        return ACTOR_GENERATOR_TABLE[ActorTypeID](ActorTypeID);
    }

    SLUG_ASSERT_MSG(false, core::StringUtility::Sprintfs("Cant Find Actor Generator [%s]", ActorTypeID.id.c_str()).c_str());
    return nullptr;
}

}
