#pragma once

namespace slug::engine_core
{

enum class UpdatePhase
{
    First,
    PrePhysics,
    PostPhysics,
    PreAnim,
    PostAnim,
    PreRender,
    PostRender,
    Last,
};

struct UpdateInfo
{
    UpdatePhase phase;
    float deltaTime;
};
}
