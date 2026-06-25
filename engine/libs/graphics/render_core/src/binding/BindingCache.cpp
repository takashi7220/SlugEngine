#include "render_core/binding/BindingCache.hpp"
#include "render_core/utility/Utility.hpp"
#include "render_core/service/RenderService.hpp"

namespace slug::render_core
{

rhi::BindingLayoutPtr BindingCache::CreateBindingLayout(const RenderService& service, const BindingCacheParam& param)
{
    rhi::BindingLayoutDesc layout_desc = {};
    layout_desc.visibility = param.shaderType;
    layout_desc.registerSpace = param.registerSpace;

    for (const auto& item : param.bindings)
    {
        rhi::BindingLayoutItem layoutItem = {};
        layoutItem.slot = SlotUtility::GetSlotIndex(item.slot.slotName);

        switch (item.type)
        {
        case BindingResourceType::ConstantBuffer:
            layoutItem.type = rhi::ResourceType::ConstantBuffer;
            break;
        case BindingResourceType::Sampler:
            layoutItem.type = rhi::ResourceType::Sampler;
            break;
        case BindingResourceType::Texture:
            layoutItem.type = rhi::ResourceType::Texture_SRV;
            break;
        default:
            return nullptr;
        }
        layout_desc.bindings.push_back(layoutItem);
    }

    return  service.GetDevice()->CreateBindingLayout(layout_desc);
}

rhi::BindingSetPtr BindingCache::CreateBindingSet(const RenderService& service, const BindingCacheParam& param, const ShaderParameter* shaderParameter, const rhi::BindingLayoutPtr& bindingLayout)
{
    rhi::BindingSetDesc binding_set_desc;
    binding_set_desc.trackLiveness = param.trackLiveness;

    for (const auto& item : param.bindings)
    {
        rhi::BindingSetItem setItem;
        uint32_t slotIndex = SlotUtility::GetSlotIndex(item.slot.slotName);
        if (item.type == BindingResourceType::ConstantBuffer)
        {
            setItem = rhi::BindingSetItem::ConstantBuffer(slotIndex, shaderParameter->GetConstantBuffer());
        }
        else
        {
            TexturePtr texture = nullptr;
            if (shaderParameter->GetTexture(item.slot, texture))
            {
                if (item.type == BindingResourceType::Texture)
                {
                    setItem = rhi::BindingSetItem::Sampler(slotIndex, texture->GetSampler());
                }
                else if (item.type == BindingResourceType::Sampler)
                {
                    setItem = rhi::BindingSetItem::Texture_SRV(slotIndex, texture->GetTexture());
                }
            }
        }
        binding_set_desc.bindings.push_back(setItem);
    }
    return service.GetDevice()->CreateBindingSet(binding_set_desc, bindingLayout);
}

BindingCachePtr BindingCache::Create(RenderService& service, const BindingCacheParam& param)
{
    BindingCachePtr binding = nullptr;
    if (!service.GetResourcePool().GetBindingCachePool()->Get(param, binding))
    {
        binding = core::NewObject<BindingCache>(param);
        service.GetResourcePool().GetBindingCachePool()->Add(param, binding);
    }
    return binding;

}

BindingCache::BindingCache(const BindingCacheParam& param)
    : m_param(param)
{
}

rhi::IBindingLayout* BindingCache::GetBindingLayout(const RenderService& service)
{
    if (m_bindingLayout)
    {
        return m_bindingLayout;
    }

    m_bindingLayout = CreateBindingLayout(service, m_param);
    return m_bindingLayout;
}


rhi::IBindingSet* BindingCache::GetBindingSet(const RenderService& service, const ShaderParameter* shaderParameter)
{
    rhi::BindingSetPtr& bindingSet = m_bindingSets[shaderParameter->GetName().data()];
    if (bindingSet)
    {
        return bindingSet;
    }

    bindingSet = CreateBindingSet(service, m_param,  shaderParameter, m_bindingLayout);
    return bindingSet;
}

rhi::BindingSetItem BindingCache::GetTextureBindingSetItem(uint32_t slot, const TexturePtr& texture)
{
    if (texture->GetTexture() && texture->GetTexture().get())
    {
        return rhi::BindingSetItem::Texture_SRV(slot, texture->GetTexture().get());
    }
    SLUG_ASSERT(false);
    return rhi::BindingSetItem {};

}

void BindingCache::Clear()
{
    m_bindingLayout.reset();
    for (auto it : m_bindingSets)
    {
        it.second.reset();
    }
}

}
