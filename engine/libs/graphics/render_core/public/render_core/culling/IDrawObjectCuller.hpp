#pragma once

#include "render_core/object/DrawObject.hpp"
#include "core/memory/ReferencePtr.hpp"
#include "core/container/Queue.hpp"

namespace slug::render_core
{

struct CullingSettings
{
    
};

class IDrawObjectCuller : public core::ReferenceObject
{
public:
    virtual void Culling(const core::TQueue<DrawObjectPtr>& drawObjects,const CullingSettings& settings) = 0;
    virtual DrawObjectPtr GetNextDrawObject() = 0;

private:
};

using IDrawObjectCullerPtr = core::TReferencePtr<IDrawObjectCuller>;

}
