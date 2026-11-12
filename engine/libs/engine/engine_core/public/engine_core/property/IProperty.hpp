#pragma once

#include "core/object/SObject.hpp"

namespace slug::engine_core
{

class IProperty : public core::SObject
{
public:
    SLUG_DECLARE_SOBJECT_TYPEINFO(IProperty, core::SObject);
};
}
