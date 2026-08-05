#pragma once

#include "render_core/resource/BufferPool.hpp"
#include "render_core/resource/TexturePool.hpp"
#include "render_core/resource/SamplerPool.hpp"
#include "render_core/resource/BindingCachePool.hpp"
#include "render_core/resource/GraphicsPipelinePool.hpp"
#include "render_core/resource/ComputePipelinePool.hpp"
#include "render_core/resource/ShaderPool.hpp"

namespace slug::render_core
{

class ResourcePool : public core::ReferenceObject
{
public:
    ResourcePool();
    virtual ~ResourcePool();

    BufferPoolPtr& GetBufferPool();
    TexturePoolPtr& GetTexturePool();
    SamplerPoolPtr& GetSamplerPool();
    BindingCachePoolPtr& GetBindingCachePool();
    GraphicsPipelinePoolPtr& GetGraphicsPipelinePool();
    ComputePipelinePoolPtr& GetComputePipelinePool();
    ShaderPoolPtr& GetShaderPool();

private:
    BufferPoolPtr m_bufferPool = nullptr;
    TexturePoolPtr m_texturePool = nullptr;
    SamplerPoolPtr m_samplerPool = nullptr;
    BindingCachePoolPtr m_bindingCachePool = nullptr;
    GraphicsPipelinePoolPtr m_graphicsPipelinePool = nullptr;
    ComputePipelinePoolPtr m_computePipelinePool = nullptr;
    ShaderPoolPtr m_shaderPool = nullptr;
};

}
