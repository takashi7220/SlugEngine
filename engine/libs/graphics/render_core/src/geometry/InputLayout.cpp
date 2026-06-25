#include "render_core/geometry/InputLayout.hpp"

namespace slug::render_core
{

InputLayoutPtr InputLayout::Create(RenderService& service, const core::TVector<rhi::VertexBufferBinding> bindings, const rhi::ShaderPtr vertexShader)
{
    core::TVector<rhi::VertexAttributeDesc> vertexAttributeDesc = {};
    vertexAttributeDesc.resize(bindings.size());
    for (auto& binding : bindings)
    {
        rhi::VertexAttributeDesc& attribute = vertexAttributeDesc.emplace_back();
        attribute.name = binding.shaderLayoutName;
        attribute.bufferIndex = binding.slot;
        attribute.arraySize = 1;
        attribute.format = binding.format;
        attribute.elementStride = binding.stride;
    }

    InputLayoutPtr inputLayout = core::NewObject<InputLayout>();
    inputLayout->m_inputLayout = service.GetDevice()->CreateInputLayout(vertexAttributeDesc.data(), static_cast<uint32_t>(vertexAttributeDesc.size()), vertexShader);
    return inputLayout;
}

const rhi::InputLayoutPtr& InputLayout::GetInputLayout()
{
    return m_inputLayout;
}

}
