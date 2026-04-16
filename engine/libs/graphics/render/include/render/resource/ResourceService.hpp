#pragma once

#include "render/resource/BufferPool.hpp"
#include "render/resource/TexturePool.hpp"
#include "render/resource/SamplerPool.hpp"
#include "render/resource/BindingCachePool.hpp"
#include "render/resource/GraphicsPipelinePool.hpp"
#include "render/resource/ComputePipelinePool.hpp"
#include "render/resource/ShaderPool.hpp"
#include "core/service/ServiceProvider.hpp"

namespace slug::render
{

class ResourceService : public core::IService
{
public:
    SLUG_SERVICE_REGISTER(ResourceService)

    ResourceService();
    virtual ~ResourceService();

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
using ResourceServicePtr = core::TReferencePtr<ResourceService>;

}
