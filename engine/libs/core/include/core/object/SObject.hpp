#pragma once

#include "core/uuid/UUID.hpp"
#include "core/memory/MemoryUtilities.hpp"
#include "math/Math.hpp"
#include "core/memory/ReferencePtr.hpp"

namespace slug
{
namespace core
{
class SObject : public core::ReferenceObject
{
public:
    core::StringView GetName() const
    {
        return m_name.c_str();
    }

    void SetName(core::StringView name)
    {
        size_t len = math::TMin(name.size(), sizeof(m_name) - 1);
        core::MemoryUtilities::Memcpy(m_name.data(), m_name.size(), name.data(), len);
        m_name[len] = '\0';
    }

    core::UUID GetUUID()
    {
        return m_uuid;
    }

private:
    core::UUID m_uuid;
    core::FixedString<64> m_name;
};
}
}
