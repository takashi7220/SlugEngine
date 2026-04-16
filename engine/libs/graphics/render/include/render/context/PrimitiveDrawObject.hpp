#pragma once

#include "core/memory/ReferencePtr.hpp"
#include "core/container/Queue.hpp"
#include "render/context/DrawContext.hpp"
#include "render/context/DrawObject.hpp"
#include "render/utility/View.hpp"

namespace slug::render
{

struct PrimitiveDrawSettings
{
    render::DrawContextPtr context;
    render::ViewPtr view;
};

class PrimitiveDrawObject : public core::ReferenceObject
{
public:
    PrimitiveDrawObject() = default;
    virtual ~PrimitiveDrawObject() = default;

    core::TQueue<render::DrawObjectPtr> drawObjects;
};
using PrimitiveDrawObjectPtr = core::TReferencePtr<PrimitiveDrawObject>;

}
