#pragma once

#include "render/material/Material.hpp"
#include "engine_core/asset/AssetUtility.hpp"

namespace slug::engine_graphics
{

class ShaderParser
{
public:
    struct TextureSlot
    {
        core::FixedString<64> slotName;
        engine_core::AssetID textureId;
    };

    struct InputParam
    {
        render::DrawContext* context;
        void* data;
        size_t size;
    };

    struct OutParam
    {
        render::ShaderPtr shader;
        core::TVector<TextureSlot> textureSlots;
    };

    static bool Load(const InputParam& input, OutParam& out);
};

class MaterialParser
{
public:
    struct ShaderSlot
    {
        engine_core::AssetID shaderId;
    };

    struct InputParam
    {
        render::DrawContext* context = nullptr;
        void* data = nullptr;
        size_t size = 0;
    };

    struct OutParam
    {
        render::MaterialPtr material;
        core::TVector<ShaderParser::TextureSlot> textureSlots;
        ShaderSlot shaderSlot;
    };

    static bool Load(const InputParam& input, OutParam& out);
};

}
