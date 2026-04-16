#pragma once

#include "render/material/ShaderParameter.hpp"


namespace slug::render
{

struct ShaderItem
{
    core::String variationName;
    rhi::ShaderPtr shader;
};

class Shader : public ShaderParameter
{
public:
    struct SetupParam
    {
        ShaderType type = ShaderType::Vertex;
        void* code = nullptr;
        size_t size = 0;
        core::StringView shaderName = "";
        core::StringView variationName = "";
    };

public:
    bool AddShader(render::DrawContext& context, const SetupParam& param);

    bool GetVertexShader(rhi::ShaderPtr& out) const;
    bool GetVertexShader(bool instancing, rhi::ShaderPtr& out) const;
    bool GetPixelShader(rhi::ShaderPtr& out) const;
    bool GetPixelShader(core::TVector<StaticSwitches>& staticSwiches, const DrawSettings& drawSettings, rhi::ShaderPtr& out) const;
    bool GetGeometryShader(rhi::ShaderPtr& out) const;
    bool GetDomainShader(rhi::ShaderPtr& out) const;
    bool GetHullShader(rhi::ShaderPtr& out) const;
    bool GetComputeShader(rhi::ShaderPtr& out) const;

    bool GetVertexShader(core::StringView name, rhi::ShaderPtr& out) const;
    bool GetPixelShader(core::StringView name, rhi::ShaderPtr& out) const;
    bool GetGeometryShader(core::StringView name, rhi::ShaderPtr& out) const;
    bool GetDomainShader(core::StringView name, rhi::ShaderPtr& out) const;
    bool GetHullShader(core::StringView name, rhi::ShaderPtr& out) const;
    bool GetComputeShader(core::StringView name, rhi::ShaderPtr& out) const;

    bool HasVertexShader(core::StringView name) const;
    bool HasPixelShader(core::StringView name) const;
    bool HasGeometryShader(core::StringView name) const;
    bool HasDomainShader(core::StringView name) const;
    bool HasHullShader(core::StringView name) const;
    bool HasComputeShader(core::StringView name) const;
private:
    bool GetShader(core::StringView name, const core::TVector<ShaderItem>& shaders, rhi::ShaderPtr& shader) const;
    bool HasShader(core::StringView name, const core::TVector<ShaderItem>& shaders) const;

private:
    core::TVector<ShaderItem> m_vertexShaders = {};
    core::TVector<ShaderItem> m_pixelShaders = {};
    core::TVector<ShaderItem> m_geometryShaders = {};
    core::TVector<ShaderItem> m_domainShaders = {};
    core::TVector<ShaderItem> m_hullShaders = {};
    core::TVector<ShaderItem> m_computeShaders = {};
};
using ShaderPtr = core::TReferencePtr<Shader>;

}
