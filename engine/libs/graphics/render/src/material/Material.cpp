#include "render/material/Material.hpp"
#include "core/memory/MemoryUtilities.hpp"
#include "core/utility/Hash.hpp"
#include "render/context/DrawContext.hpp"
#include "render/utility/Utility.hpp"

namespace slug::render
{

namespace
{

}

void Material::SetShader(ShaderPtr shader)
{
    m_shader = shader;
}

const ShaderPtr Material::GetShader() const
{
    return m_shader;
}

void Material::Reflesh(DrawContext& context)
{
    ConstantParameter constantParameter = {};
    GetOverrideConstantParameter(constantParameter);
    UpdateBindingResource(context, constantParameter);
}

bool Material::GetParameter(core::StringView name, bool& value) const
{
    if (!ShaderParameter::GetParameter(name, value))
    {
        return m_shader->GetParameter(name, value);
    }
    return false;
}

bool Material::GetParameter(core::StringView name, float& value) const
{
    if (!ShaderParameter::GetParameter(name, value))
    {
        return m_shader->GetParameter(name, value);
    }
    return false;
}

bool Material::GetParameter(core::StringView name, core::TVector<float>& values) const
{
    if (!ShaderParameter::GetParameter(name, values))
    {
        return m_shader->GetParameter(name, values);
    }
    return false;
}

bool Material::GetParameter(core::StringView name, math::Vector2& value) const
{
    if (!ShaderParameter::GetParameter(name, value))
    {
        return m_shader->GetParameter(name, value);
    }
    return false;
}

bool Material::GetParameter(core::StringView name, math::Vector3& value) const
{
    if (!ShaderParameter::GetParameter(name, value))
    {
        return m_shader->GetParameter(name, value);
    }
    return false;
}

bool Material::GetParameter(core::StringView name, math::Vector4& value) const
{
    if (!ShaderParameter::GetParameter(name, value))
    {
        return m_shader->GetParameter(name, value);
    }
    return false;
}

bool Material::GetParameter(core::StringView name, math::Matrix3x3& value) const
{
    if (!ShaderParameter::GetParameter(name, value))
    {
        return m_shader->GetParameter(name, value);
    }
    return false;
}

bool Material::GetParameter(core::StringView name, math::Matrix4x4& value) const
{
    if (!ShaderParameter::GetParameter(name, value))
    {
        return m_shader->GetParameter(name, value);
    }
    return false;
}

bool Material::GetTexture(const SlotInfo& slot, TexturePtr& texture) const
{
    if (!ShaderParameter::GetTexture(slot, texture))
    {
        return m_shader->GetTexture(slot, texture);
    }
    return false;
}

void Material::GetStaticSwitches(core::TVector<StaticSwitches>& staticSwitches) const
{
    m_shader->GetStaticSwitches(staticSwitches);
    for (auto& staticSwitch : staticSwitches)
    {
        bool value = staticSwitch.value;
        if (GetStaticSwitch(staticSwitch.name, value))
        {
            staticSwitch.value = value;
        }
    }
}

bool Material::GetStaticSwitch(core::StringView name, bool& value) const
{
    if (!ShaderParameter::GetStaticSwitch(name, value))
    {
        return m_shader->GetStaticSwitch(name, value);
    }
    return false;
}

void Material::GetOverrideConstantParameter(ConstantParameter& constantParameter)
{
    if (m_shader == nullptr)
    {
        constantParameter = *this;
    }
    else
    {
        constantParameter = *m_shader;
    }

    for (auto accessor : GetAccessors())
    {
        if (accessor.type == ParameterType::Bool)
        {
            bool value = false;
            if (GetParameter(accessor.name, value))
            {
                constantParameter.SetParameter(accessor.name, value);
            }
        }
        else if (accessor.type == ParameterType::Float)
        {
            float value = 0.0f;
            if (GetParameter(accessor.name, value))
            {
                constantParameter.SetParameter(accessor.name, value);
            }
        }
        else if (accessor.type == ParameterType::FloatArray)
        {
            core::TVector<float> values = {};
            if (GetParameter(accessor.name, values))
            {
                constantParameter.SetParameter(accessor.name, values.data(), values.size());
            }
        }
        else if (accessor.type == ParameterType::Float2)
        {
            math::Vector2 value = {};
            if (GetParameter(accessor.name, value))
            {
                constantParameter.SetParameter(accessor.name, value);
            }
        }
        else if (accessor.type == ParameterType::Float3)
        {
            math::Vector3 value = {};
            if (GetParameter(accessor.name, value))
            {
                constantParameter.SetParameter(accessor.name, value);
            }
        }
        else if (accessor.type == ParameterType::Float4)
        {
            math::Vector4 value = {};
            if (GetParameter(accessor.name, value))
            {
                constantParameter.SetParameter(accessor.name, value);
            }
        }
        else if (accessor.type == ParameterType::Float3x3)
        {
            math::Matrix3x3 value = {};
            if (GetParameter(accessor.name, value))
            {
                constantParameter.SetParameter(accessor.name, value);
            }
        }
        else if (accessor.type == ParameterType::Float4x4)
        {
            math::Matrix4x4 value = {};
            if (GetParameter(accessor.name, value))
            {
                constantParameter.SetParameter(accessor.name, value);
            }
        }
    }
}

}
