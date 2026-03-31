#pragma once
#include "render/geometry/Model.hpp"
#include "render/context/DrawContext.hpp"
#include "engine_core/asset/AssetUtility.hpp"

namespace slug
{
namespace engine_graphics
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
        render::DrawContext* context;
        void* data;
        size_t size;
    };

    struct OutParam
    {
        render::ModelPtr model = nullptr;
        core::TVector<MaterialSlot> materialSlots = {};
    };

    static bool Parse(const InputParam& input, OutParam& out);
};

}
}
