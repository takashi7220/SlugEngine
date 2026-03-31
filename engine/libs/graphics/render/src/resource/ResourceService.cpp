#include "render/resource/ResourceService.hpp"

namespace slug::render
{

ResourceService::ResourceService()
{
    m_bufferPool.reset(new BufferPool());
    m_texturePool.reset(new TexturePool());
    m_samplerPool.reset(new SamplerPool());
    m_bindingCachePool.reset(new BindingCachePool());
    m_graphicsPipelinePool.reset(new GraphicsPipelinePool());
    m_computePipelinePool.reset(new ComputePipelinePool());
    m_shaderPool.reset(new ShaderPool());
}

ResourceService::~ResourceService()
{

}


BufferPoolPtr& ResourceService::GetBufferPool()
{
    return m_bufferPool;
}

TexturePoolPtr& ResourceService::GetTexturePool()
{
    return m_texturePool;
}

SamplerPoolPtr& ResourceService::GetSamplerPool()
{
    return m_samplerPool;
}

BindingCachePoolPtr& ResourceService::GetBindingCachePool()
{
    return m_bindingCachePool;
}

GraphicsPipelinePoolPtr& ResourceService::GetGraphicsPipelinePool()
{
    return m_graphicsPipelinePool;
}

ComputePipelinePoolPtr& ResourceService::GetComputePipelinePool()
{
    return m_computePipelinePool;
}

ShaderPoolPtr& ResourceService::GetShaderPool()
{
    return m_shaderPool;
}

}
