#pragma once

#include <cstdint>
#include "core/container/String.hpp"

namespace slug
{
namespace engine_graphics
{

enum class ReflectionMemberType_PrimitiveComponent : uint8_t
{
};

inline core::StringView GetReflectionMemberName_PrimitiveComponent(ReflectionMemberType_PrimitiveComponent type)
{
    return "";
}

enum class ReflectionMemberType_MeshComponent : uint8_t
{
    m_model = 0,
    m_materials = 1,
};

inline core::StringView GetReflectionMemberName_MeshComponent(ReflectionMemberType_MeshComponent type)
{
    static const core::StringView ReflectionMemberNames[] =
    {
        "m_model",
        "m_materials"
    };
    return ReflectionMemberNames[(size_t)type];
}


}
}
