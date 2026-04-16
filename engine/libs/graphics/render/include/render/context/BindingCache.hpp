#pragma once

#include "rhi/Rhi.hpp"
#include "core/container/Vector.hpp"
#include "core/container/UnorderedMap.hpp"
#include "render/material/Material.hpp"
namespace slug::render
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

class DrawContext;

class BindingCache : public core::ReferenceObject
{
public:
    static BindingCache* Create(DrawContext& context, BindingCacheParam& param);
    static rhi::BindingLayoutPtr CreateBindingLayout(const DrawContext& context,const BindingCacheParam& param);
    static rhi::BindingSetPtr CreateBindingSet(const DrawContext& context,const BindingCacheParam& param, const ShaderParameter* shaderParameter, const rhi::BindingLayoutPtr& bindingLayout);

public:
    BindingCache(const BindingCacheParam& param);

    rhi::IBindingLayout* GetBindingLayout(const DrawContext& context);
    rhi::IBindingSet* GetBindingSet(const DrawContext& context, const ShaderParameter* shaderParameter);
    rhi::BindingSetItem GetTextureBindingSetItem(uint32_t slot, const TexturePtr& texture);

    void Clear();
private:
    rhi::BindingLayoutPtr m_bindingLayout = {};
    core::TUnorderedMap<core::String, rhi::BindingSetPtr> m_bindingSets = {};
    BindingCacheParam m_param = {};
};
using BindingCachePtr = core::TReferencePtr<BindingCache>;

}
