#pragma once
#include "engine_core/world/IWorld.hpp"
#include "engine_core/level/Level.hpp"


namespace slug::engine_core
{
class World : public IWorld
{
public:
    LevelPtr GetPersistantLevel();
    void SetPersistantLevel(const LevelPtr& level);

    void SpawnActor(core::UUID levelID, const SpawnParameter& spawnParameter);


private:
    LevelPtr m_persistentLevel;
};

}
