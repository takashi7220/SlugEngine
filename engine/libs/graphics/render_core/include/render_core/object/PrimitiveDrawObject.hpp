#pragma once

#include "render_core/object/DrawObject.hpp"
#include "core/memory/ReferencePtr.hpp"
#include "core/container/Queue.hpp"

namespace slug::render_core
{

class PrimitiveDrawObject : public core::ReferenceObject
{
public:
    PrimitiveDrawObject() = default;
    virtual ~PrimitiveDrawObject() = default;

    core::TQueue<render_core::DrawObjectPtr> drawObjects;
};
using PrimitiveDrawObjectPtr = core::TReferencePtr<PrimitiveDrawObject>;

}
