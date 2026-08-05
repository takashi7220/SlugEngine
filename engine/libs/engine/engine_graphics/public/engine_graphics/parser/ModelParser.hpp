#pragma once
#include "render_core/geometry/Model.hpp"
#include "render_core/service/RenderService.hpp"
#include "engine_core/asset/AssetUtility.hpp"

namespace slug::engine_graphics
{

class ModelParser
{
public:
    struct MaterialSlot
    {
        core::FixedString<128> slotName;
        engine_core::AssetID materialId;
    };

    struct InputParam
    {
        render_core::DrawContext* context;
        void* data;
        size_t size;
    };

    struct OutParam
    {
        render_core::ModelPtr model = nullptr;
        core::TVector<MaterialSlot> materialSlots = {};
    };

    static bool Parse(const InputParam& input, OutParam& out);
};

}
