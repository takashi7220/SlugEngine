#pragma once

#include "render_core/service/RenderService.hpp"
#include "rhi/Rhi.hpp"


namespace slug::render_core
{

class InputLayout;
using InputLayoutPtr = core::TReferencePtr<InputLayout>;

class InputLayout : public core::ReferenceObject
{
public:
    static InputLayoutPtr Create(RenderService& service, const core::TVector<rhi::VertexBufferBinding> bindings, const rhi::ShaderPtr vertexShader);

public:
    const rhi::InputLayoutPtr& GetInputLayout();

private:
    rhi::InputLayoutPtr m_inputLayout;
};


}
