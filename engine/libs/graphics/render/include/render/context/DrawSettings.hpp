#pragma once

#include "core/container/String.hpp"

namespace slug
{
namespace render
{
enum class BlendOptions : uint8_t
{
    Opaque,
    Mask,
    Transparent,
    Addtive,
    Modulate,
};

struct GiSettings
{
    float diffuseBoost = 1.0f;
    float emissiveBoost = 1.0f;
};

struct ShadingModel
{
    core::String name = "DefaultLit";
    uint32_t id = 0;
};

struct RenderPass
{
    core::String blendPass = "";
    core::String shadingModelPass = "";
};

struct DrawSettings
{
    BlendOptions blendOptions = BlendOptions::Opaque;
    ShadingModel shadingModel = {};
    GiSettings giSettings = {};
    float alphaThreshold = 0.5f;
    int32_t sortPriority = 0;
    bool twoSide = false;

    RenderPass GetRenderPass()
    {
        RenderPass pass;
        switch (this->blendOptions)
        {
        case BlendOptions::Opaque:
            pass.blendPass = "Opaque";
            break;
        case BlendOptions::Mask:
            pass.blendPass = "Mask";
            break;
        case BlendOptions::Transparent:
            pass.blendPass = "Transparent";
            break;
        }
        pass.blendPass;
        pass.shadingModelPass = this->shadingModel.name;
        return pass;
    }
};

}
}
