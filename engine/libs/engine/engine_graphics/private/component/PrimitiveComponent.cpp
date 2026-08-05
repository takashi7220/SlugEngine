#include "engine_graphics/component/PrimitiveComponent.hpp"
#include "render_core/utility/Utility.hpp"
namespace slug::engine_graphics
{

SLUG_COMPONENT_REGISTER(PrimitiveComponent)

PrimitiveComponent::PrimitiveComponent()
{

}

void PrimitiveComponent::GatherDrawObject(const render_core::PrimitiveDrawSettings& settings, render_core::PrimitiveDrawObjectPtr& primitiveDrawObject)
{
    SLUG_ASSERT("Please overload the derived function.");
}

math::AABB PrimitiveComponent::GetBoundingBox(bool applyWorldMatrix)
{
    return math::AABB();
}

void PrimitiveComponent::UpdateShaderParameter(const render_core::PrimitiveDrawSettings& settings)
{
    render_core::ConstantBufferUtility::ObjectConstantParameter param;
    param.world = this->GetWorldMatrix();
    param.preWorld = this->GetPreWorldMatrix();
    param.worldNormal = param.world.Adjoint();
    param.aabb = GetBoundingBox(false);

    if (m_shaderParameter == nullptr)
    {
        m_shaderParameter = new render_core::ShaderParameter();
        m_shaderParameter->SetName(this->GetName());
        render_core::ConstantBufferUtility::UpdateObjectShaderParameter(m_shaderParameter, param, false);
    }
    else
    {
        render_core::ConstantBufferUtility::UpdateObjectShaderParameter(m_shaderParameter, param, true);
    }
    m_shaderParameter->Reflesh(*settings.context);
}

render_core::DrawObject::Settings GetSettings()
{
    return render_core::DrawObject::Settings();
}

}
