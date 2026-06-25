#pragma once

#include "render_core/material/Material.hpp"
#include "render_core/utility/PostProcessSettings.hpp"
#include "rhi/Rhi.hpp"
#include "render_core/Define.hpp"
#include "algorithm/primitive/Aabb.hpp"

namespace slug::render_core
{

class DrawContext;
class View;
using ViewPtr = core::TReferencePtr<View>;

class View : public RenderResource
{
public:
    static ViewPtr Create(RenderService& service, core::StringView name);

    View();
    virtual ~View();

    void Reflesh(RenderService& service);

    rhi::ViewportState GetViewportState() const;
    rhi::VariableRateShadingState GetVariableRateShadingState() const;
    const rhi::BindingLayoutPtr& GetBindingLayout() const;
    const rhi::BindingSetPtr& GetBindingSet() const;
    rhi::TextureSubresourceSet GetSubresource() const;

    void SetViewMatrix(const math::Matrix4x4& view);
    void SetProjectionMatrix(const math::Matrix4x4& projection);
    void SetResolution(float width, float height);
    void SetCameraPosition(const math::Vector3& position);

    const math::Matrix4x4& GetViewMatrix() const;
    const math::Matrix4x4& GetInvViewMatrix() const;
    const math::Matrix4x4& GetPreViewMatrix() const;

    const math::Matrix4x4& GetProjectionMatrix() const;
    const math::Matrix4x4& GetInvProjectionMatrix() const;
    const math::Matrix4x4& GetPreProjectionMatrix() const;

    const math::Matrix4x4& GetViewProjectionMatrix() const;
    const math::Matrix4x4& GetInvViewProjectionMatrix() const;
    const math::Matrix4x4& GetPreViewProjectionMatrix() const;

    const PostprocessSettings& GetPostprocessSettings() const;

    float CalculateDrawPixelRate(const algorithm::Aabb& boundingBox) const;
private:
    void UpdateViewProjectionMatrix();

private:
    math::Matrix4x4 m_viewMatrix = {};
    math::Matrix4x4 m_invViewMatrix = {};
    math::Matrix4x4 m_preViewMatrix = {};

    math::Matrix4x4 m_projectionMatrix = {};
    math::Matrix4x4 m_invProjectionMatrix = {};
    math::Matrix4x4 m_preProjectionMatrix = {};

    math::Matrix4x4 m_viewProjectionMatrix = {};
    math::Matrix4x4 m_invViewProjectionMatrix = {};
    math::Matrix4x4 m_preViewProjectionMatrix = {};

    math::Vector2 m_resolution = {};
    math::Vector3 m_cameraPosition = {};

    rhi::Viewport m_viewport = {};
    rhi::Rect m_siccorRect = {};
    rhi::VariableRateShadingState m_variableRateShadingState = {};

    PostprocessSettings m_postProcessSettings = {};

    ShaderParameterPtr m_shaderParameter = nullptr;
    rhi::BindingLayoutPtr m_bindingLayout = nullptr;
    rhi::BindingSetPtr m_bindingSet = nullptr;
};

}
