#pragma once

#include "render/context/DrawObject.hpp"

namespace slug::render
{

struct CullingSettings
{
    
};

// DrawObjectに対してカリングを行うためのクラス
class IDrawObjectCuller : public core::ReferenceObject
{
public:
    virtual void Culling(const core::TQueue<DrawObjectPtr>& drawObjects,const CullingSettings& settings) = 0;
    virtual DrawObjectPtr GetNextDrawObject() = 0;

private:
};

using IDrawObjectCullerPtr = core::TReferencePtr<IDrawObjectCuller>;

}
