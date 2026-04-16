#pragma once

#include "render/context/DrawContext.hpp"
#include "rhi/Rhi.hpp"


namespace slug::render
{


class InputLayout : public core::ReferenceObject
{
public:
    static InputLayout* Create(DrawContext& context, const core::TVector<rhi::VertexBufferBinding> bindings, const rhi::ShaderPtr vertexShader);

public:
    const rhi::InputLayoutPtr& GetInputLayout();

private:
    rhi::InputLayoutPtr m_inputLayout;
};
using InputLayoutPtr = core::TReferencePtr<InputLayout>;


}
