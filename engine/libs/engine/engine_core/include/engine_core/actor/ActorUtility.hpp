#pragma once

#include "core/container/String.hpp"
#include "core/utility/Hash.hpp"
#include "core/uuid/NameID.hpp"

namespace slug::engine_core
{

struct ActorTypeID
{
    core::NameID id;

    ActorTypeID() = default;
    ActorTypeID(core::StringView _id)
        : id(_id)
    {
    }

    core::StringView GetName()
    {
        return id.GetStr();
    }

    bool operator==(const ActorTypeID& v0) const
    {
        return id == v0.id;
    }
};

struct ActorTypeIDKeyHash
{
    size_t operator()(const ActorTypeID& key) const
    {
        return key.id.GetHash();
    }
};

struct ActorTypeIDKeyCompare
{
    size_t operator()(const ActorTypeID& v0, const ActorTypeID& v1) const
    {
        return v0 == v1;
    }
};

class Actor;
using GetActorTypeIDPtr = ActorTypeID(*)();
using GenerateActorPtr = Actor* (*)(const ActorTypeID&);

class ActorUtility
{
public:
    static void RegisterGenerator(GetActorTypeIDPtr getFunc, GenerateActorPtr createFunc);
    static bool HasGenerator(const ActorTypeID& actorTypeId);
    static Actor* Generate(const ActorTypeID& actorTypeId);
};
}
