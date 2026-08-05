#pragma once
#include "render_core/geometry/Mesh.hpp"
#include "core/object/SObject.hpp"
#include "math/Transform.hpp"
#include "render_core/utility/View.hpp"
#include "render_core/Define.hpp"

namespace slug::render_core
{

struct LodItemSettings
{
    float drawPixelAreaRatio = 0.0f;
};

struct LodSettings
{
    uint32_t offsteLodIndex = 0;
    core::TVector<LodItemSettings> items = {};
};

class SubModel : public RenderResource
{
public:
    BoundingBox boundingBox = {};
    core::TVector<MeshPtr> meshes = {};
};
using SubModelPtr = core::TReferencePtr<SubModel>;


class LodModel : public RenderResource
{
public:
    LodSettings lodSettings = {};
    core::TVector<SubModelPtr> subModels = {};
};
using LodModelPtr = core::TReferencePtr<LodModel>;


class Model : public RenderResource
{
public:
    SubModelPtr GetSubModel(const math::Matrix4x4& worldMatrix, const render_core::ViewPtr& view);

    SubModelPtr mainModel;
    LodModelPtr lodModel;
};
using ModelPtr = core::TReferencePtr<Model>;

}
