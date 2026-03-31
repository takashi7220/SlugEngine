#include "render/context/DrawContext.hpp"
#include "render/utility/View.hpp"
#include "render/utility/Utility.hpp"
#include "shader/Constants.h"
#include "rhi/common/Utility.hpp"
namespace slug
{
namespace render
{

View* View::Create(DrawContext& context, core::StringView name)
{
    View* view = new View();
    view->SetName(name);

    BindingCacheParam param = {};
    param.shaderType = rhi::ShaderType::All;
    {
        BindingResource& binding = param.bindings.emplace_back();
        binding.type = BindingResourceType::ConstantBuffer;
        binding.slot.slotName = SlotUtility::GetViewSlotName();
    }
    BindingCachePtr binding = BindingCache::Create(context, param);

    view->m_shaderParameter.reset(new ShaderParameter());
    view->m_shaderParameter->SetName(view->GetName());
    for (auto& viewParam : shader::ViewConstants)
    {
        view->m_shaderParameter->AddParameter(viewParam.name.c_str(), (render::ParameterType)viewParam.type, 1, viewParam.size, nullptr);
    }
    view->m_shaderParameter->Reflesh(context);

    view->m_bindingLayout = binding->GetBindingLayout(context);
    view->m_bindingSet = binding->GetBindingSet(context, view->m_shaderParameter);
    return view;
}

View::View()
{

}

View::~View()
{
}

void View::Reflesh(DrawContext& context)
{
    m_shaderParameter->SetParameter("view", m_viewMatrix);
    m_shaderParameter->SetParameter("viewProjection", m_viewProjectionMatrix);
    m_shaderParameter->SetParameter("invView", m_invViewMatrix);
    m_shaderParameter->SetParameter("invProjection", m_invProjectionMatrix);
    m_shaderParameter->SetParameter("invViewProjection", m_invViewProjectionMatrix);
    m_shaderParameter->SetParameter("preViewProjection", m_preViewProjectionMatrix);
    m_shaderParameter->SetParameter("cameraPosition", m_cameraPosition);
    m_shaderParameter->SetParameter("resolution", m_resolution);
    m_shaderParameter->SetParameter("invResolution", math::Vector2(1.0f / m_resolution.x, 1.0f / m_resolution.y));
    m_shaderParameter->Reflesh(context);
}

rhi::ViewportState View::GetViewportState() const
{
    rhi::ViewportState state = {};
    state.viewports.push_back(m_viewport);
    state.scissorRects.push_back(m_siccorRect);
    return state;
}

rhi::VariableRateShadingState View::GetVariableRateShadingState() const
{
    return m_variableRateShadingState;
}

const rhi::BindingLayoutPtr& View::GetBindingLayout() const
{
    return m_bindingLayout;
}

const rhi::BindingSetPtr& View::GetBindingSet() const
{
    return m_bindingSet;
}

rhi::TextureSubresourceSet View::GetSubresource() const
{
    return rhi::TextureSubresourceSet(0, 1, 0, 1);
}

void View::SetViewMatrix(const math::Matrix4x4& view)
{
    m_preViewMatrix = m_viewMatrix;
    m_viewMatrix = view;
    m_invViewMatrix = m_viewMatrix.Inverse();
    UpdateViewProjectionMatrix();
}

void View::SetProjectionMatrix(const math::Matrix4x4& projection)
{
    m_preProjectionMatrix = projection;
    m_projectionMatrix = projection;
    m_invProjectionMatrix = m_projectionMatrix.Inverse();
    UpdateViewProjectionMatrix();
}

void View::SetResolution(float width, float height)
{
    m_resolution.x = width;
    m_resolution.y = height;
    m_viewport = rhi::Viewport(width, height);
    m_siccorRect = rhi::Rect(m_viewport);
}

void View::SetCameraPosition(const math::Vector3& position)
{
    m_cameraPosition = position;
}

const math::Matrix4x4& View::GetViewMatrix() const
{
    return m_viewMatrix;
}

const math::Matrix4x4& View::GetInvViewMatrix() const
{
    return m_invViewMatrix;
}

const math::Matrix4x4& View::GetPreViewMatrix() const
{
    return m_preViewMatrix;
}

const math::Matrix4x4& View::GetProjectionMatrix() const
{
    return m_projectionMatrix;
}

const math::Matrix4x4& View::GetInvProjectionMatrix() const
{
    return m_invProjectionMatrix;
}

const math::Matrix4x4& View::GetPreProjectionMatrix() const
{
    return m_preProjectionMatrix;
}

const math::Matrix4x4& View::GetViewProjectionMatrix() const
{
    return m_viewProjectionMatrix;
}

const math::Matrix4x4& View::GetInvViewProjectionMatrix() const
{
    return m_invViewProjectionMatrix;
}

const math::Matrix4x4& View::GetPreViewProjectionMatrix() const
{
    return m_preViewProjectionMatrix;
}

const PostprocessSettings& View::GetPostprocessSettings() const
{
    return m_postProcessSettings;
}

float View::CalculateDrawPixelRate(const math::AABB& aabb) const
{
    math::Vector3 center = aabb.Center();
    float radius = aabb.Length() * 0.5f;

    math::Vector4 clipPos = m_viewProjectionMatrix * math::Vector4(center);
    if (math::TEqual(clipPos.w, 0.0f))
    {
        return 0.0f;
    }

    float p11 = m_viewProjectionMatrix.m[5];
    float radiusPixel = math::TAbs(radius) * math::TAbs(p11) * m_resolution.y / math::TAbs(clipPos.w);
    radiusPixel = (2.0f * radiusPixel) / m_resolution.y;
    return radiusPixel;
}

void View::UpdateViewProjectionMatrix()
{
    m_preViewProjectionMatrix = m_viewProjectionMatrix;
    m_viewProjectionMatrix = m_projectionMatrix * m_viewMatrix;
    m_invViewProjectionMatrix = m_viewProjectionMatrix.Inverse();
}

}
}
