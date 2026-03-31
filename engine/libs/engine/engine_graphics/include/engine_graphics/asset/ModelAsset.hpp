#pragma once

#include "engine_graphics/asset/GraphicsAssetBase.hpp"
#include "engine_graphics/asset/MaterialAsset.hpp"
#include "engine_core/asset/AssetBase.hpp"
#include "render/geometry/Model.hpp"


namespace slug
{
namespace engine_graphics
{

class ModelAsset : public GraphicsAssetBase
{
public:
    struct MaterialSlot
    {
        core::FixedString<128> slotName;
        engine_core::AssetID materialId;
        MaterialAssetPtr material;
    };

public:
    SLUG_GRAPHICS_ASSET_MEMBER_DEFINE(ModelAsset)

    ModelAsset(engine_core::AssetHeader header, render::DrawContext& context);
    bool Load(void* data, size_t size) override;
    bool Resolve(const engine_core::AssetResolver& resolver) override;
    bool Reload(void* data, size_t size) override;
    void Destroy() override;

public:
    const render::ModelPtr& GetModel() const;
    void GetMaterialSlotNames(core::TVector<core::FixedString<128>>& slotNames, bool withLod = true) const;
    const MaterialAssetPtr GetMaterial(core::StringView slotName) const;
    void SetMaterial(core::StringView slotName, const MaterialAssetPtr& material);

private:
    void UpdateMaterial();

private:
    core::TVector<MaterialSlot> m_materialSlots;
    render::ModelPtr m_model;
};

using ModelAssetPtr = core::TReferencePtr<ModelAsset>;
}
}
