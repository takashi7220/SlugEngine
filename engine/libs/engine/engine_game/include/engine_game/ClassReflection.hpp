#pragma once

#include <cstdint>
#include "core/container/String.hpp"
#include "core/reflection/ReflectionClass.hpp"

namespace slug
{
namespace engine_game
{

enum class ReflectionMemberType_TransformComponent : uint8_t
{
    m_transform = 0
};

inline core::StringView GetReflectionMemberName_TransformComponent(ReflectionMemberType_TransformComponent type)
{
    static const core::StringView ReflectionMemberNames[] =
    {
        "m_transform",
    };
    return ReflectionMemberNames[(size_t)type];
}

}
}
