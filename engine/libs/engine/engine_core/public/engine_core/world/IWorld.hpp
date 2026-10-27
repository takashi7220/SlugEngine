#pragma once

#include "core/object/SObject.hpp"

namespace slug::engine_core
{

class IWorld : public core::SObject
{
public:
    SLUG_DECLARE_SOBJECT_TYPEINFO(IWorld, core::SObject)
};
}
