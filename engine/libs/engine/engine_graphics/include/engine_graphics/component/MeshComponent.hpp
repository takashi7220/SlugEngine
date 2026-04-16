#pragma once
#include "engine_graphics/ClassReflection.hpp"
#include "engine_graphics/component/PrimitiveComponent.hpp"
#include "engine_graphics/asset/ModelAsset.hpp"
#include "engine_graphics/asset/MaterialAsset.hpp"

namespace slug::engine_graphics
{

class MeshComponent : public PrimitiveComponent
{
public:
    SLUG_DERIVED_COMPONENT_MEMBER(MeshComponent, PrimitiveComponent)

public:
    struct MaterialSlot
    {
        core::FixedString<128> slotName;
        MaterialAssetPtr material;
    };

    MeshComponent();
    virtual void GatherDrawObject(const render::PrimitiveDrawSettings& settings, render::PrimitiveDrawObjectPtr& primitiveDrawObject) override;
    virtual math::AABB GetBoundingBox(bool applyWorldMatrix = true) override;

    MaterialAssetPtr GetMaterialAsset(core::StringView slotName);

private:
    ModelAssetPtr m_model;
    core::TVector<MaterialSlot> m_materialSlots;

};

}
