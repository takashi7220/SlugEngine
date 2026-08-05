#pragma once

#include "engine_graphics/asset/GraphicsAssetBase.hpp"
#include "engine_core/asset/AssetBase.hpp"
#include "render_core/texture/Texture.hpp"


namespace slug::engine_graphics
{

class TextureAsset : public GraphicsAssetBase
{
public:
    SLUG_GRAPHICS_ASSET_MEMBER_DEFINE(TextureAsset)

    TextureAsset(engine_core::AssetHeader header, render_core::RenderService& service);
    bool Load(void* data, size_t size) override;
    bool Resolve(const engine_core::AssetResolver& resolver) override;
    bool Reload(void* data, size_t size) override;
    void Destroy() override;

    const render_core::TexturePtr& GetTexture();

private:
    render_core::TexturePtr m_texture;
};

using TextureAssetPtr = core::TReferencePtr<TextureAsset>;
}
