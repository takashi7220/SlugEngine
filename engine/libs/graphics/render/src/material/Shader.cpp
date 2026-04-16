#include "render/material/Shader.hpp"
#include "render/utility/Utility.hpp"
#include "render/context/DrawContext.hpp"
#include "core/memory/MemoryUtilities.hpp"

namespace slug::render
{

bool Shader::AddShader(render::DrawContext& context, const SetupParam& param)
{
    
    auto& shaderPool = context.GetResourceService()->GetShaderPool();
    core::String shaderName = core::StringUtility::Sprintfs("%s_%s", param.shaderName, param.variationName).c_str();
    uint64_t key = core::HashUtility::ConvertStringToHash64(shaderName.c_str());

    rhi::ShaderPtr shader = nullptr;
    if (!shaderPool->Get(key, shader))
    {
        rhi::ShaderDesc desc = {};
        desc.shaderType = static_cast<rhi::ShaderType>(param.type);
        desc.debugName = shaderName;
        shader = context.GetDevice()->CreateShader(desc, param.code, param.size);
        shaderPool->Add(key, shader);
    }

    if (param.type == ShaderType::Vertex)
    {
        if (!HasVertexShader(param.variationName))
        {
            auto& vs = m_vertexShaders.emplace_back();
            vs.shader = shader;
            vs.variationName = param.variationName;
        }
    }
    else if (param.type == ShaderType::Pixel)
    {
        if (!HasPixelShader(param.variationName))
        {
            auto& ps = m_pixelShaders.emplace_back();
            ps.shader = shader;
            ps.variationName = param.variationName;
        }
    }
    else if (param.type == ShaderType::Geometry)
    {
        if (!HasGeometryShader(param.variationName))
        {
            auto& gs = m_geometryShaders.emplace_back();
            gs.shader = shader;
            gs.variationName = param.variationName;
        }
    }
    else if (param.type == ShaderType::Hull)
    {
        if (!HasHullShader(param.variationName))
        {
            auto& hs = m_hullShaders.emplace_back();
            hs.shader = shader;
            hs.variationName = param.variationName;
        }
    }
    else if (param.type == ShaderType::Domain)
    {
        if (!HasDomainShader(param.variationName))
        {
            auto& ds = m_domainShaders.emplace_back();
            ds.shader = shader;
            ds.variationName = param.variationName;
        }
    }
    else if (param.type == ShaderType::Compute)
    {
        if (!HasComputeShader(param.variationName))
        {
            auto& cs = m_computeShaders.emplace_back();
            cs.shader = shader;
            cs.variationName = param.variationName;
        }
    }
    return true;
}

bool Shader::GetVertexShader(rhi::ShaderPtr& out) const
{
    return GetVertexShader(ShaderVariationUtility::GetDefaultShaderName(), out);
}

bool Shader::GetVertexShader(bool instancing, rhi::ShaderPtr& out) const
{
    return GetVertexShader(ShaderVariationUtility::GetVertexShaderName(instancing), out);
}

bool Shader::GetPixelShader(rhi::ShaderPtr& out) const
{
    return GetPixelShader(ShaderVariationUtility::GetDefaultShaderName(), out);
}

bool Shader::GetPixelShader(core::TVector<StaticSwitches>& staticSwiches, const DrawSettings& drawSettings, rhi::ShaderPtr& out) const
{
    return GetPixelShader(ShaderVariationUtility::GetPixelShaderName(staticSwiches, drawSettings), out);
}

bool Shader::GetGeometryShader(rhi::ShaderPtr& out) const
{
    return GetGeometryShader(ShaderVariationUtility::GetDefaultShaderName(), out);
}

bool Shader::GetDomainShader(rhi::ShaderPtr& out) const
{
    return GetDomainShader(ShaderVariationUtility::GetDefaultShaderName(), out);
}

bool Shader::GetHullShader(rhi::ShaderPtr& out) const
{
    return GetHullShader(ShaderVariationUtility::GetDefaultShaderName(), out);
}

bool Shader::GetComputeShader(rhi::ShaderPtr& out) const
{
    return GetComputeShader(ShaderVariationUtility::GetDefaultShaderName(), out);
}


bool Shader::GetVertexShader(core::StringView name, rhi::ShaderPtr& out) const
{
    return GetShader(name, m_vertexShaders, out);
}

bool Shader::GetPixelShader(core::StringView name, rhi::ShaderPtr& out) const
{
    return GetShader(name, m_pixelShaders, out);
}

bool Shader::GetGeometryShader(core::StringView name, rhi::ShaderPtr& out) const
{
    return GetShader(name, m_geometryShaders, out);
}

bool Shader::GetDomainShader(core::StringView name, rhi::ShaderPtr& out) const
{
    return GetShader(name, m_domainShaders, out);
}

bool Shader::GetHullShader(core::StringView name, rhi::ShaderPtr& out) const
{
    return GetShader(name, m_hullShaders, out);
}

bool Shader::GetComputeShader(core::StringView name, rhi::ShaderPtr& out) const
{
    return GetShader(name, m_computeShaders, out);
}

bool Shader::HasVertexShader(core::StringView name) const
{
    return HasShader(name, m_vertexShaders);
}

bool Shader::HasPixelShader(core::StringView name) const
{
    return HasShader(name, m_pixelShaders);
}

bool Shader::HasGeometryShader(core::StringView name) const
{
    return HasShader(name, m_geometryShaders);
}

bool Shader::HasDomainShader(core::StringView name) const
{
    return HasShader(name, m_domainShaders);
}

bool Shader::HasHullShader(core::StringView name) const
{
    return HasShader(name, m_hullShaders);
}

bool Shader::HasComputeShader(core::StringView name) const
{
    return HasShader(name, m_computeShaders);
}

bool Shader::GetShader(core::StringView name, const core::TVector<ShaderItem>& shaders, rhi::ShaderPtr& shader) const
{
    for (auto& shaderItem : shaders)
    {
        if (shaderItem.variationName == name)
        {
            shader = shaderItem.shader;
            return true;
        }
    }
    return false;
}

bool Shader::HasShader(core::StringView name, const core::TVector<ShaderItem>& shaders) const
{
    for (auto& shaderItem : shaders)
    {
        if (shaderItem.variationName == name)
        {
            return true;
        }
    }
    return false;
}

}
