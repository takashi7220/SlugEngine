#pragma once

#include "render_core/geometry/Mesh.hpp"
#include "render_core/Define.hpp"
#include "core/memory/ReferencePtr.hpp"
#include "core/uuid/UUID.hpp"

namespace slug::render_core
{
class DrawObject;
using DrawObjectPtr = core::TReferencePtr<DrawObject>;

using DrawObjectID = core::UUID;

class DrawObject : public RenderResource
{
public:
    struct Settings
    {
        uint8_t stencilValue;
    };

public:
    static DrawObjectPtr Create(
        const Settings& settings,
        const render_core::ShaderParameterPtr& shaderParameter,
        const render_core::MeshPtr& mesh,
        const render_core::MaterialPtr& material);

    SLUG_NODISCARD const MeshPtr& GetMesh() const noexcept;
    SLUG_NODISCARD const MaterialPtr& GetMaterial() const noexcept;
    SLUG_NODISCARD const Settings& GetSettings() const noexcept;
    SLUG_NODISCARD const rhi::BindingLayoutPtr& GetBindingLayout() const noexcept;
    SLUG_NODISCARD const rhi::BindingSetPtr& GetBindingSet() const noexcept;
    SLUG_NODISCARD const DrawObjectID& GetDrawObjectID() const noexcept;

private:
    DrawObjectID m_id = {};
    MeshPtr m_mesh = nullptr;
    ShaderParameterPtr m_shaderParameter = nullptr;
    MaterialPtr m_material = nullptr;
    Settings m_settings = {};
};

}
