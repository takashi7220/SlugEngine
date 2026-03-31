#pragma once

#include "engine_graphics/asset/GraphicsAssetBase.hpp"
#include "engine_core/asset/AssetBase.hpp"
#include "render/texture/Texture.hpp"


namespace slug
{
namespace engine_graphics
{

class TextureAsset : public GraphicsAssetBase
{
public:
    SLUG_GRAPHICS_ASSET_MEMBER_DEFINE(TextureAsset)

    TextureAsset(engine_core::AssetHeader header, render::DrawContext& context);
    bool Load(void* data, size_t size) override;
    bool Resolve(const engine_core::AssetResolver& resolver) override;
    bool Reload(void* data, size_t size) override;
    void Destroy() override;

    const render::TexturePtr& GetTexture();

private:
    render::TexturePtr m_texture;
};

using TextureAssetPtr = core::TReferencePtr<TextureAsset>;
}
}
