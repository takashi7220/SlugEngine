#pragma once

#include "engine_graphics/asset/GraphicsAssetBase.hpp"
#include "engine_graphics/asset/TextureAsset.hpp"
#include "engine_core/asset/AssetBase.hpp"
#include "render_core/material/Shader.hpp"


namespace slug::engine_graphics
{

class ShaderAsset : public GraphicsAssetBase
{
public:
    struct TextureSlot
    {
        core::FixedString<128> slotName = {};
        engine_core::AssetID textureId;
        TextureAssetPtr texture = nullptr;
    };

public:
    SLUG_GRAPHICS_ASSET_MEMBER_DEFINE(ShaderAsset)

    ShaderAsset(engine_core::AssetHeader header, render_core::RenderService& service);
    bool Load(void* data, size_t size) override;
    bool Resolve(const engine_core::AssetResolver& resolver) override;
    bool Reload(void* data, size_t size) override;
    void Destroy() override;

    const render_core::ShaderPtr& GetShader();

private:
    render_core::ShaderPtr m_shader;
    core::TVector<TextureSlot> m_textureSlots;
};

using ShaderAssetPtr = core::TReferencePtr<ShaderAsset>;
}
