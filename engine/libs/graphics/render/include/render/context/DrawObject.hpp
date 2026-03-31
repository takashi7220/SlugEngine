#pragma once

#include "core/memory/ReferencePtr.hpp"
#include "core/container/String.hpp"

#include "render/geometry/Mesh.hpp"

namespace slug
{
namespace render
{

class DrawObject : public core::SObject
{
public:
    struct Settings
    {
        uint8_t stencilValue;
    };

public:

    static DrawObject* Create(const Settings& settings, const render::ShaderParameterPtr& shaderParameter, const render::MeshPtr& mesh, const render::MaterialPtr& material)
    {
        DrawObject* drawObject = new DrawObject();
        if (drawObject == nullptr)
        {
            return nullptr;
        }

        drawObject->m_settings = settings;
        drawObject->m_shaderParameter = shaderParameter;
        drawObject->m_mesh = mesh;
        drawObject->m_material = material;
        return drawObject;
    }

    MeshPtr GetMesh() const
    {
        return m_mesh;
    }

    MaterialPtr GetMaterial() const
    {
        if (m_material != nullptr)
        {
            return m_material;
        }
        return m_mesh->GetMaterial();
    }

    const Settings& GetSettings() const
    {
        return m_settings;
    }

    const rhi::BindingLayoutPtr& GetBindingLayout() const
    {
        return m_shaderParameter->GetBindingLayout();
    }

    const rhi::BindingSetPtr& GetBindingSet() const
    {
        return m_shaderParameter->GetBindingSet();
    }

private:
    MeshPtr m_mesh = nullptr;
    ShaderParameterPtr m_shaderParameter = nullptr;
    MaterialPtr m_material = nullptr;
    Settings m_settings = {};
};
using DrawObjectPtr = core::TReferencePtr<DrawObject>;

}
}
