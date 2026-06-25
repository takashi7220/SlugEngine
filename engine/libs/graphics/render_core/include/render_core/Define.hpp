#pragma once

#include "core/object/SObject.hpp"

namespace slug::render_core
{
class RenderResource : public core::SObject
{
public:
    SLUG_DECLARE_SOBJECT_TYPEINFO(RenderResource, core::SObject)
    core::StringView GetName() const
    {
        return m_name.c_str();
    }

    void SetName(core::StringView name)
    {
        m_name = name;
    }

private:
    core::FixedString<128> m_name;
};
}
