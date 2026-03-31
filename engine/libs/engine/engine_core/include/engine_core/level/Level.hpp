#pragma once

#include "engine_core/actor/Actor.hpp"
#include "engine_core/world/IWorld.hpp"
#include "core/container/List.hpp"

namespace slug
{
namespace engine_core
{

class Level;
using LevelPtr = core::TReferencePtr<Level>;

class World;

class Level : public core::SObject
{
public:
    Level();

    void SetWorld(IWorld* world);
    IWorld* GetWorld();

    void SetParentLevel(Level* parent);
    Level* GetParentLevel();

    void AddChildLevel(Level* level);
    void DeleteChildLevel(core::UUID uuid);
    bool HasChildLevel(core::UUID uuid);
    LevelPtr GetChild(core::UUID uuid);
    void GetChildren(core::TVector<Level*>& children, bool recursive = false);

    void AddActor(Actor* actor);
    void DeleteActor(core::UUID uuid);
    bool HasActor(core::UUID uuid);
    ActorPtr GetActor(core::UUID uuid);
    void GetActors(core::TVector<Actor*>& actors);

    void EnumerateActors(core::TVector<Actor*>& actors);
    void EnumerateLevels(core::TVector<Level*>& levels);
private:
    IWorld* m_world;
    Level* m_parent;
    core::TList<LevelPtr> m_childLevels;
    core::TList<ActorPtr> m_actors;
};
}
}
