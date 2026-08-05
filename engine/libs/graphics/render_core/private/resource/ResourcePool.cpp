#include "render_core/resource/ResourcePool.hpp"

namespace slug::render_core
{

ResourcePool::ResourcePool()
{
    m_bufferPool.reset(core::NewObject<BufferPool>());
    m_texturePool.reset(new TexturePool());
    m_samplerPool.reset(new SamplerPool());
    m_bindingCachePool.reset(new BindingCachePool());
    m_graphicsPipelinePool.reset(new GraphicsPipelinePool());
    m_computePipelinePool.reset(new ComputePipelinePool());
    m_shaderPool.reset(new ShaderPool());
}

ResourcePool::~ResourcePool()
{

}


BufferPoolPtr& ResourcePool::GetBufferPool()
{
    return m_bufferPool;
}

TexturePoolPtr& ResourcePool::GetTexturePool()
{
    return m_texturePool;
}

SamplerPoolPtr& ResourcePool::GetSamplerPool()
{
    return m_samplerPool;
}

BindingCachePoolPtr& ResourcePool::GetBindingCachePool()
{
    return m_bindingCachePool;
}

GraphicsPipelinePoolPtr& ResourcePool::GetGraphicsPipelinePool()
{
    return m_graphicsPipelinePool;
}

ComputePipelinePoolPtr& ResourcePool::GetComputePipelinePool()
{
    return m_computePipelinePool;
}

ShaderPoolPtr& ResourcePool::GetShaderPool()
{
    return m_shaderPool;
}

}
