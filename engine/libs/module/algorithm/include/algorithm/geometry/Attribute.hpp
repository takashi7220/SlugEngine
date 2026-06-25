#pragma once

#include "core/object/SObject.hpp"

namespace slug::algorithm
{

enum class AttributeType
{
    Empty,
    Mesh,
};

struct Attribute : core::SObject
{
    SLUG_DECLARE_SOBJECT_TYPEINFO(Attribute, core::SObject)
    AttributeType type = AttributeType::Empty;
};
using AttributePtr = core::TReferencePtr<Attribute>;

}
