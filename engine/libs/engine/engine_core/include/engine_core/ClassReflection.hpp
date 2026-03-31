#pragma once

#include "core/reflection/ReflectionClass.hpp"

namespace slug
{
namespace engine_core
{
class SceneComponent_ReflectionClass
{
public:
    enum class MemberType : uint16_t
    {
        m_transform,
    };

    static core::StringView GetMemberName(MemberType type);
    static core::PropertyDescriptor GetDescriptor(MemberType type);
};
}
}
