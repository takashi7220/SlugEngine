#pragma once
#include "engine_core/actor/SceneComponent.hpp"
#include "engine_graphics/ClassReflection.hpp"
#include "render_core/context/PrimitiveDrawObject.hpp"

namespace slug::engine_graphics
{

class PrimitiveComponent : public engine_core::SceneComponent
{
public:
    SLUG_DERIVED_COMPONENT_MEMBER(PrimitiveComponent, engine_core::SceneComponent)

    PrimitiveComponent();
    virtual void GatherDrawObject(const render_core::PrimitiveDrawSettings& settings, render_core::PrimitiveDrawObjectPtr& primitiveDrawObject);
    virtual math::AABB GetBoundingBox(bool applyWorldMatrix = true);

protected:
    virtual void UpdateShaderParameter(const render_core::PrimitiveDrawSettings& settings);
    virtual render_core::DrawObject::Settings GetSettings();
protected:
    render_core::ShaderParameterPtr m_shaderParameter;
};
}
