#pragma once

#include "render_core/object/DrawObject.hpp"

namespace slug::render_core
{
DrawObjectPtr DrawObject::Create(
    const Settings& settings,
    const render_core::ShaderParameterPtr& shaderParameter,
    const render_core::MeshPtr& mesh,
    const render_core::MaterialPtr& material)
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

const MeshPtr& DrawObject::GetMesh() const noexcept
{
    return m_mesh;
}

const MaterialPtr& DrawObject::GetMaterial() const noexcept
{
    if (m_material != nullptr)
    {
        return m_material;
    }
    return m_mesh->GetMaterial();
}

const DrawObject::Settings& DrawObject::GetSettings() const noexcept
{
    return m_settings;
}

const rhi::BindingLayoutPtr& DrawObject::GetBindingLayout() const noexcept
{
    return m_shaderParameter->GetBindingLayout();
}

const rhi::BindingSetPtr& DrawObject::GetBindingSet() const noexcept
{
    return m_shaderParameter->GetBindingSet();
}

const DrawObjectID& DrawObject::GetDrawObjectID() const noexcept
{
    return m_id;
}

}
