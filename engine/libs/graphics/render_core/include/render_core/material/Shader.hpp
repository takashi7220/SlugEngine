#pragma once

#include "render_core/material/ShaderParameter.hpp"

namespace slug::render_core
{

struct ShaderVariation
{
    using Key = int32_t;

    struct ShaderItem
    {
        core::TVector<core::FixedString<64>> variations;
        rhi::ShaderPtr shader = nullptr;
    };

    core::TVector<core::FixedString<64>> variations = {};
    core::TVector<ShaderItem> shaderItems = {};
};

class Shader : public ShaderParameter
{
public:
    struct SetupParam
    {
        struct Item
        {
            core::TVector<core::StringView> variations = {};
            void* code = nullptr;
            size_t size = 0;
        };

        ShaderType type = ShaderType::Vertex;
        core::StringView shaderName = "";
        core::TVector<core::StringView> variations = {};
        core::TVector<Item> items = {};
    };

public:
    bool RegistShader(render_core::RenderService& service, const SetupParam& param);

    bool GetVertexShader(rhi::ShaderPtr& out) const;
    bool GetPixelShader(rhi::ShaderPtr& out) const;
    bool GetGeometryShader(rhi::ShaderPtr& out) const;
    bool GetDomainShader(rhi::ShaderPtr& out) const;
    bool GetHullShader(rhi::ShaderPtr& out) const;
    bool GetComputeShader(rhi::ShaderPtr& out) const;

private:

    bool GetVertexShader(const ShaderVariation::Key& key, rhi::ShaderPtr& out) const;
    bool GetPixelShader(const ShaderVariation::Key& key, rhi::ShaderPtr& out) const;
    bool GetGeometryShader(const ShaderVariation::Key& key, rhi::ShaderPtr& out) const;
    bool GetDomainShader(const ShaderVariation::Key& key, rhi::ShaderPtr& out) const;
    bool GetHullShader(const ShaderVariation::Key& key, rhi::ShaderPtr& out) const;
    bool GetComputeShader(const ShaderVariation::Key& key, rhi::ShaderPtr& out) const;

    bool HasVertexShader(const ShaderVariation::Key& key) const;
    bool HasPixelShader(const ShaderVariation::Key& key) const;
    bool HasGeometryShader(const ShaderVariation::Key& key) const;
    bool HasDomainShader(const ShaderVariation::Key& key) const;
    bool HasHullShader(const ShaderVariation::Key& key) const;
    bool HasComputeShader(const ShaderVariation::Key& key) const;

    static ShaderVariation::Key GetVariationKey(const ShaderParameter& shaderParameter, const ShaderVariation& shaderVariation);
    static bool RegistShader(render_core::RenderService& service, const SetupParam& param, ShaderVariation& outShaderVariation);
    static bool GetShader(const ShaderVariation::Key& key, const ShaderVariation& shaderVariation, rhi::ShaderPtr& shader);
    static bool HasShader(const ShaderVariation::Key& key, const ShaderVariation& shaderVariation);

private:
    ShaderVariation m_vertexShader = {};
    ShaderVariation m_pixelShader = {};
    ShaderVariation m_geometryShader = {};
    ShaderVariation m_domainShader = {};
    ShaderVariation m_hullShader = {};
    ShaderVariation m_computeShader = {};
};
using ShaderPtr = core::TReferencePtr<Shader>;

}
