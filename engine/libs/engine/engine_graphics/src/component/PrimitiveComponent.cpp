#include "engine_graphics/component/PrimitiveComponent.hpp"
#include "render/utility/Utility.hpp"
namespace slug::engine_graphics
{

SLUG_COMPONENT_REGISTER(PrimitiveComponent)

PrimitiveComponent::PrimitiveComponent()
{

}

void PrimitiveComponent::GatherDrawObject(const render::PrimitiveDrawSettings& settings, render::PrimitiveDrawObjectPtr& primitiveDrawObject)
{
    SLUG_ASSERT("Please overload the derived function.");
}

math::AABB PrimitiveComponent::GetBoundingBox(bool applyWorldMatrix)
{
    return math::AABB();
}

void PrimitiveComponent::UpdateShaderParameter(const render::PrimitiveDrawSettings& settings)
{
    render::ConstantBufferUtility::ObjectConstantParameter param;
    param.world = this->GetWorldMatrix();
    param.preWorld = this->GetPreWorldMatrix();
    param.worldNormal = param.world.Adjoint();
    param.aabb = GetBoundingBox(false);

    if (m_shaderParameter == nullptr)
    {
        m_shaderParameter = new render::ShaderParameter();
        m_shaderParameter->SetName(this->GetName());
        render::ConstantBufferUtility::UpdateObjectShaderParameter(m_shaderParameter, param, false);
    }
    else
    {
        render::ConstantBufferUtility::UpdateObjectShaderParameter(m_shaderParameter, param, true);
    }
    m_shaderParameter->Reflesh(*settings.context);
}

render::DrawObject::Settings GetSettings()
{
    return render::DrawObject::Settings();
}

}
