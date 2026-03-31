#pragma once

#include "engine_core/actor/Actor.hpp"

namespace slug::engine_core
{
class EmptyActor : public Actor
{
public:
    SLUG_ACTOR_MEMBER(EmptyActor)

    void Initialize() override;
    void Start() override;
    void Tick(const TickParam& tickParam) override;
    void End() override;

private:
};
}
