#pragma once

#include "rhi/Rhi.hpp"
#include "core/container/Vector.hpp"
#include "core/container/UnorderedMap.hpp"
#include "render_core/material/Material.hpp"
namespace slug::render_core
{

enum class BindingResourceType : uint8_t
{
    ConstantBuffer,
    Sampler,
    Texture
};

struct BindingResource
{
    BindingResourceType type = BindingResourceType::ConstantBuffer;
    SlotInfo slot;
};

struct BindingCacheParam
{
    core::TVector<BindingResource> bindings = {};
    rhi::ShaderType shaderType = rhi::ShaderType::None;
    uint32_t registerSpace = 0;
    uint32_t offsetRegisterSpace = 0;
    bool trackLiveness = false;
};

class RenderService;

class BindingCache;
using BindingCachePtr = core::TReferencePtr<BindingCache>;


class BindingCache : public core::ReferenceObject
{
public:
    static rhi::BindingLayoutPtr CreateBindingLayout(const RenderService& service,const BindingCacheParam& param);
    static rhi::BindingSetPtr CreateBindingSet(const RenderService& service,const BindingCacheParam& param, const ShaderParameter* shaderParameter, const rhi::BindingLayoutPtr& bindingLayout);

public:
    static BindingCachePtr Create(RenderService& service, const BindingCacheParam& param);
    BindingCache(const BindingCacheParam& param);

    rhi::IBindingLayout* GetBindingLayout(const RenderService& service);
    rhi::IBindingSet* GetBindingSet(const RenderService& service, const ShaderParameter* shaderParameter);
    rhi::BindingSetItem GetTextureBindingSetItem(uint32_t slot, const TexturePtr& texture);

    void Clear();
private:
    rhi::BindingLayoutPtr m_bindingLayout = {};
    core::TUnorderedMap<core::String, rhi::BindingSetPtr> m_bindingSets = {};
    BindingCacheParam m_param = {};
};

}
