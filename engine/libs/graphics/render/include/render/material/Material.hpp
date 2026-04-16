#pragma once

#include "render/material/Shader.hpp"

#include "rhi/Rhi.hpp"

namespace slug::render
{

class DrawContext;

class Material : public ShaderParameter
{
public:
    void SetShader(ShaderPtr shader);
    const ShaderPtr GetShader() const;

    void Reflesh(DrawContext& context) override;

    bool GetParameter(core::StringView name, bool& value) const override;
    bool GetParameter(core::StringView name, float& value) const override;
    bool GetParameter(core::StringView name, core::TVector<float>& values) const override;
    bool GetParameter(core::StringView name, math::Vector2& value) const override;
    bool GetParameter(core::StringView name, math::Vector3& value) const override;
    bool GetParameter(core::StringView name, math::Vector4& value) const override;
    bool GetParameter(core::StringView name, math::Matrix3x3& value) const override;
    bool GetParameter(core::StringView name, math::Matrix4x4& value) const override;
    bool GetTexture(const SlotInfo& slot, TexturePtr& texture) const override;
    bool GetStaticSwitch(core::StringView name, bool& value) const override;
    void GetStaticSwitches(core::TVector<StaticSwitches>& staticSwitches) const override;
private:
    void GetOverrideConstantParameter(ConstantParameter& constantParameter);

private:
    ShaderPtr m_shader;

};
using MaterialPtr = core::TReferencePtr<Material>;


}
