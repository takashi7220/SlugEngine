#include "render_core/geometry/Model.hpp"

namespace slug::render_core
{
SubModelPtr Model::GetSubModel(const math::Matrix4x4& worldMatrix, const render_core::ViewPtr& view)
{
    if (lodModel != nullptr)
    {
        BoundingBox worldBoundingBox = mainModel->boundingBox * worldMatrix;
        float drawPixelRate = view->CalculateDrawPixelRate(worldBoundingBox);
        uint32_t begin = lodModel->lodSettings.offsteLodIndex;

        int32_t index = -1;
        for (uint32_t i = begin; i < lodModel->lodSettings.items.size(); i++)
        {
            const LodItemSettings& item = lodModel->lodSettings.items[i];
            if (item.drawPixelAreaRatio > drawPixelRate)
            {
                index = static_cast<int32_t>(i);
            }
        }

        if (index > 0)
        {
            return lodModel->subModels.at(index);
        }
    }
    return mainModel;
}
}
