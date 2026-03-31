#pragma once
#include "render/geometry/Mesh.hpp"
#include "core/object/SObject.hpp"
#include "math/Transform.hpp"
#include "render/utility/View.hpp"

namespace slug
{
namespace render
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

class SubModel : public core::SObject
{
public:
    math::AABB boundingBox = {};
    core::TVector<MeshPtr> meshes = {};
};
using SubModelPtr = core::TReferencePtr<SubModel>;


class LodModel : public core::SObject
{
public:
    LodSettings lodSettings = {};
    core::TVector<SubModelPtr> subModels = {};
};
using LodModelPtr = core::TReferencePtr<LodModel>;


class Model : public core::SObject
{
public:
    SubModelPtr GetSubModel(const math::Matrix4x4& worldMatrix, const render::ViewPtr& view)
    {
        if (lodModel == nullptr)
        {
            return mainModel;
        }

        math::AABB worldAABB = worldMatrix * mainModel->boundingBox;
        float drawPixelRate = view->CalculateDrawPixelRate(worldAABB);
        uint32_t begin = lodModel->lodSettings.offsteLodIndex;

        int32_t index = -1;
        for (uint32_t i = begin; i < lodModel->lodSettings.items.size(); i++)
        {
            LodItemSettings& item = lodModel->lodSettings.items[i];
            if (item.drawPixelAreaRatio > drawPixelRate)
            {
                index = static_cast<int32_t>(i);
            }
        }

        if (index < 0)
        {
            return mainModel;
        }
        else
        {
            return lodModel->subModels.at(index);
        }
    }

    SubModelPtr mainModel;
    LodModelPtr lodModel;
};
using ModelPtr = core::TReferencePtr<Model>;

}
}
