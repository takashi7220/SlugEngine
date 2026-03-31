#include "render/material/ShaderParameter.hpp"
#include "core/memory/MemoryUtilities.hpp"
#include "render/context/DrawContext.hpp"
#include "render/utility/Utility.hpp"

namespace slug
{
namespace render
{

void ShaderParameter::Reflesh(DrawContext& context)
{
    UpdateBindingResource(context, *this);
}

void ShaderParameter::GetTextureSlots(core::TVector<SlotInfo>& slots)
{
    for (auto& itr : m_textures)
    {
        slots.push_back(itr.slot);
    }
}

bool ShaderParameter::SetTexture(const SlotInfo& slot, TexturePtr texture)
{
    for (auto& itr : m_textures)
    {
        if (itr.slot == slot)
        {
            itr.texture = texture;
            return true;
        }
    }
    return false;
}

void ShaderParameter::AddTexture(const SlotInfo& slot, TexturePtr texture)
{
    if (!HasTexture(slot))
    {
        auto& tmpTexture = m_textures.emplace_back();
        tmpTexture.slot = slot;
        tmpTexture.texture = texture;
    }
}

bool ShaderParameter::GetTexture(const SlotInfo& slot, TexturePtr& texture) const
{
    for (auto& itr : m_textures)
    {
        if (itr.slot == slot)
        {
            texture = itr.texture;
            return true;
        }
    }
    return false;

}

bool ShaderParameter::HasTexture(const SlotInfo& slot) const
{
    for (auto& itr : m_textures)
    {
        if (itr.slot == slot)
        {
            return true;
        }
    }
    return false;
}

void ShaderParameter::GetStaticSwitches(core::TVector<StaticSwitches>& staticSwiches) const
{
    staticSwiches.clear();
    for (auto& staticSwitch : m_staticSwitches)
    {
        staticSwiches.push_back(staticSwitch);
    }
}

bool ShaderParameter::SetStaticSwitch(core::StringView name, bool value)
{
    for (auto& itr : m_staticSwitches)
    {
        if (itr.name == name)
        {
            itr.value = value;
            return true;
        }
    }
    return false;
}

void ShaderParameter::AddStaticSwitches(core::StringView name, bool value)
{
    if (!HasStaticSwitches(name))
    {
        auto& staticSwich = m_staticSwitches.emplace_back();
        staticSwich.name = name;
        staticSwich.value = value;
    }
}

bool ShaderParameter::GetStaticSwitch(core::StringView name, bool& value) const
{
    for (auto& itr : m_staticSwitches)
    {
        if (itr.name == name)
        {
            value = itr.value;
            return true;
        }
    }
    return false;
}

bool ShaderParameter::HasStaticSwitches(core::StringView name) const
{
    for (auto& itr : m_staticSwitches)
    {
        if (itr.name == name)
        {
            return true;
        }
    }
    return false;
}

void ShaderParameter::SetDrawSetttings(const DrawSettings& settings)
{
    m_drawSettings = settings;
}

const DrawSettings& ShaderParameter::GetDrawSettings() const
{
    return m_drawSettings;
}

const rhi::BufferPtr& ShaderParameter::GetConstantBuffer() const
{
    return m_constantBuffer;
}

const rhi::BindingLayoutPtr& ShaderParameter::GetBindingLayout() const
{
    return m_bindingLayout;
}

const rhi::BindingSetPtr& ShaderParameter::GetBindingSet() const
{
    return m_bindingSet;
}

void ShaderParameter::UpdateBindingResource(DrawContext& context, const ConstantParameter& constantParameter)
{
    uint64_t key = core::HashUtility::ConvertStringToHash64(this->GetName());
    m_constantBuffer = nullptr;
    auto service = context.GetResourceService();
    service->GetBufferPool()->Get(key, m_constantBuffer);

    if (m_constantBuffer != nullptr && m_needCreateBuffer)
    {
        service->GetBufferPool()->Delete(key);
        m_constantBuffer = nullptr;
    }

    if (m_constantBuffer == nullptr)
    {
        rhi::BufferDesc bufferDesc = {};
        bufferDesc.byteSize = sizeof(constantParameter.GetCostantBufferSize());
        bufferDesc.debugName = this->GetName();
        bufferDesc.useConstantBuffer = true;
        bufferDesc.initialState = rhi::ResourceStates::ConstantBuffer;
        bufferDesc.keepInitialState = true;

        m_constantBuffer = context.GetDevice()->CreateBuffer(bufferDesc);
        service->GetBufferPool()->Add(key, m_constantBuffer);
    }

    if (m_needUpdateBuffer)
    {
        context.GetCreateResourceCommand()->WriteBuffer(m_constantBuffer, constantParameter.GetConstantBufferPtr().data(), constantParameter.GetCostantBufferSize());
    }

    BindingCacheParam param = {};
    param.shaderType = rhi::ShaderType::All;
    {
        BindingResource& binding = param.bindings.emplace_back();
        binding.type = BindingResourceType::ConstantBuffer;
        binding.slot.slotName = SlotUtility::GetMaterialSlotName();
    }

    core::TVector<SlotInfo> textureSlotInfos = {};
    GetTextureSlots(textureSlotInfos);

    for (auto textuerSlot : textureSlotInfos)
    {
        BindingResource& binding = param.bindings.emplace_back();
        binding.type = BindingResourceType::Texture;
        binding.slot.slotName = textuerSlot.slotName;
    }

    BindingCachePtr binding = BindingCache::Create(context, param);

    if (binding == nullptr)
    {
        return;
    }

    m_bindingLayout = binding->GetBindingLayout(context);
    m_bindingSet = binding->GetBindingSet(context, this);

    m_needCreateBuffer = false;
    m_needUpdateBuffer = false;
}

}
}
