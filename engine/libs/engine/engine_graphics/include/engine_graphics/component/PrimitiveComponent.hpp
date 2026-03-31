#pragma once
#include "engine_core/actor/SceneComponent.hpp"
#include "engine_graphics/ClassReflection.hpp"
#include "render/context/PrimitiveDrawObject.hpp"

namespace slug::engine_graphics
{

class PrimitiveComponent : public engine_core::SceneComponent
{
public:
    SLUG_DERIVED_COMPONENT_MEMBER(PrimitiveComponent, engine_core::SceneComponent)

    PrimitiveComponent();
    virtual void GatherDrawObject(const render::PrimitiveDrawSettings& settings, render::PrimitiveDrawObjectPtr& primitiveDrawObject);
    virtual math::AABB GetBoundingBox(bool applyWorldMatrix = true);

protected:
    virtual void UpdateShaderParameter(const render::PrimitiveDrawSettings& settings);
    virtual render::DrawObject::Settings GetSettings();
protected:
    render::ShaderParameterPtr m_shaderParameter;
};
}
