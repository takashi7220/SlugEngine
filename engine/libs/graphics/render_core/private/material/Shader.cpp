#include "render_core/material/Shader.hpp"
#include "render_core/utility/Utility.hpp"
#include "render_core/service/RenderService.hpp"
#include "core/memory/MemoryUtility.hpp"

namespace slug::render_core
{

namespace
{
static core::String VariationsToString(const core::TVector<core::StringView>& variations)
{
    core::String ret = {};
    for (size_t i = 0; i < variations.size(); i++)
    {
        if (i != 0)
        {
            ret += "|";
            ret += variations[i];
        }
        else
        {
            ret = variations[i];
        }
    }
    return ret;
}

}

bool Shader::RegistShader(render_core::RenderService& service, const SetupParam& param)
{
    switch (param.type)
    {
    case ShaderType::Vertex:
        RegistShader(service, param, m_vertexShader);
        break;
    case ShaderType::Pixel:
        RegistShader(service, param, m_pixelShader);
        break;
    case ShaderType::Geometry:
        RegistShader(service, param, m_geometryShader);
        break;
    case ShaderType::Hull:
        RegistShader(service, param, m_hullShader);
        break;
    case ShaderType::Domain:
        RegistShader(service, param, m_domainShader);
        break;
    case ShaderType::Compute:
        RegistShader(service, param, m_computeShader);
        break;
    default:
        break;
    }
    return true;
}

bool Shader::GetVertexShader(rhi::ShaderPtr& out) const
{
    ShaderVariation::Key key = GetVariationKey(*this, m_vertexShader);
    return GetShader(key, m_vertexShader, out);
}

bool Shader::GetPixelShader(rhi::ShaderPtr& out) const
{
    ShaderVariation::Key key = GetVariationKey(*this, m_pixelShader);
    return GetShader(key, m_pixelShader, out);
}

bool Shader::GetGeometryShader(rhi::ShaderPtr& out) const
{
    ShaderVariation::Key key = GetVariationKey(*this, m_geometryShader);
    return GetShader(key, m_geometryShader, out);
}

bool Shader::GetDomainShader(rhi::ShaderPtr& out) const
{
    ShaderVariation::Key key = GetVariationKey(*this, m_domainShader);
    return GetShader(key, m_domainShader, out);
}

bool Shader::GetHullShader(rhi::ShaderPtr& out) const
{
    ShaderVariation::Key key = GetVariationKey(*this, m_hullShader);
    return GetShader(key, m_hullShader, out);
}

bool Shader::GetComputeShader(rhi::ShaderPtr& out) const
{
    ShaderVariation::Key key = GetVariationKey(*this, m_computeShader);
    return GetShader(key, m_computeShader, out);
}

bool Shader::GetVertexShader(const ShaderVariation::Key& key, rhi::ShaderPtr& out) const
{
    return GetShader(key, m_vertexShader, out);
}

bool Shader::GetPixelShader(const ShaderVariation::Key& key, rhi::ShaderPtr& out) const
{
    return GetShader(key, m_pixelShader, out);
}

bool Shader::GetGeometryShader(const ShaderVariation::Key& key, rhi::ShaderPtr& out) const
{
    return GetShader(key, m_geometryShader, out);
}

bool Shader::GetDomainShader(const ShaderVariation::Key& key, rhi::ShaderPtr& out) const
{
    return GetShader(key, m_domainShader, out);
}

bool Shader::GetHullShader(const ShaderVariation::Key& key, rhi::ShaderPtr& out) const
{
    return GetShader(key, m_hullShader, out);
}

bool Shader::GetComputeShader(const ShaderVariation::Key& key, rhi::ShaderPtr& out) const
{
    return GetShader(key, m_computeShader, out);
}

bool Shader::HasVertexShader(const ShaderVariation::Key& key) const
{
    return HasShader(key, m_vertexShader);
}

bool Shader::HasPixelShader(const ShaderVariation::Key& key) const
{
    return HasShader(key, m_pixelShader);
}

bool Shader::HasGeometryShader(const ShaderVariation::Key& key) const
{
    return HasShader(key, m_geometryShader);
}

bool Shader::HasDomainShader(const ShaderVariation::Key& key) const
{
    return HasShader(key, m_domainShader);
}

bool Shader::HasHullShader(const ShaderVariation::Key& key) const
{
    return HasShader(key, m_hullShader);
}

bool Shader::HasComputeShader(const ShaderVariation::Key& key) const
{
    return HasShader(key, m_computeShader);
}

ShaderVariation::Key Shader::GetVariationKey(const ShaderParameter& shaderParameter, const ShaderVariation& shaderVariation)
{
    ShaderVariation::Key key = 0;
    for (size_t i = 0; i < shaderVariation.variations.size(); i++)
    {
        bool value = false;
        if (shaderParameter.GetStaticSwitch(shaderVariation.variations[i].view(), value))
        {
            if (value)
            {
                key |= 1 << i;
            }
        }
    }
    return key;
}

bool Shader::RegistShader(render_core::RenderService& service, const SetupParam& param, ShaderVariation& outShaderVariation)
{
    outShaderVariation.variations.clear();
    outShaderVariation.shaderItems.clear();

    for (auto& variation : param.variations)
    {
        outShaderVariation.variations.push_back(variation);
    }

    auto& shaderPool = service.GetResourcePool().GetShaderPool();
    for (auto& item : param.items)
    {
        core::String shaderName = core::StringUtility::Snprintf("%s|%s", param.shaderName.data(), VariationsToString(item.variations).c_str());
        uint64_t key = core::HashUtility::CreateHash64(shaderName.c_str());

        rhi::ShaderPtr shader = nullptr;
        if (!shaderPool->Get(key, shader))
        {
            rhi::ShaderDesc desc = {};
            desc.shaderType = static_cast<rhi::ShaderType>(param.type);
            desc.debugName = shaderName;
            shader = service.GetDevice()->CreateShader(desc, item.code, item.size);
            shaderPool->Add(key, shader);
        }

        if (shader != nullptr)
        {
            auto& shaderItem = outShaderVariation.shaderItems.emplace_back();
            shaderItem.shader = shader;
            for (const auto& variation : item.variations)
            {
                shaderItem.variations.push_back(variation);
            }
        }
    }
    return true;
}

bool Shader::GetShader(const ShaderVariation::Key& key, const ShaderVariation& shaderVariation, rhi::ShaderPtr& shader)
{
    if (key >= 0 && key < static_cast<int32_t>(shaderVariation.shaderItems.size()))
    {
        shader = shaderVariation.shaderItems[key].shader;
        return true;
    }
    return false;
}

bool Shader::HasShader(const ShaderVariation::Key& key, const ShaderVariation& shaderVariation)
{
    if (key >= 0 && key < static_cast<int32_t>(shaderVariation.shaderItems.size()))
    {
        return true;
    }
    return false;
}

}
