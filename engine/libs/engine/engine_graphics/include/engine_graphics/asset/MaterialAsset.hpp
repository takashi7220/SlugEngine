#pragma once

#include "engine_graphics/asset/GraphicsAssetBase.hpp"
#include "engine_graphics/asset/ShaderAsset.hpp"
#include "engine_graphics/asset/TextureAsset.hpp"
#include "engine_core/asset/AssetBase.hpp"
#include "render/material/Material.hpp"


namespace slug
{
namespace engine_graphics
{


class MaterialAsset : public GraphicsAssetBase
{
public:
    struct ShaderSlot
    {
        engine_core::AssetID shaderId;
        ShaderAssetPtr shader = nullptr;
    };

public:
    SLUG_GRAPHICS_ASSET_MEMBER_DEFINE(MaterialAsset)

    MaterialAsset(engine_core::AssetHeader header, render::DrawContext& context);
    bool Load(void* data, size_t size) override;
    bool Resolve(const engine_core::AssetResolver& resolver) override;
    bool Reload(void* data, size_t size) override;
    void Destroy() override;

    render::MaterialPtr GetMaterial();
private:
    void UpdateMaterial();

private:
    ShaderSlot m_shaderSlot;
    core::TVector<ShaderAsset::TextureSlot> m_textureSlots;
    render::MaterialPtr m_material;
};

using MaterialAssetPtr = core::TReferencePtr<MaterialAsset>;
}
}
