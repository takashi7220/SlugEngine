#pragma once
#include "engine_core/world/IWorld.hpp"
#include "engine_core/level/Level.hpp"


namespace slug::engine_core
{
class World : public IWorld
{
public:
    SLUG_DECLARE_SOBJECT_TYPEINFO(World, IWorld)

private:

};

}
