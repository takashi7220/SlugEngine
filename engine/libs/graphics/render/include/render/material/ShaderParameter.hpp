#pragma once

#include "render/material/DrawSettings.hpp"
#include "render/texture/Texture.hpp"
#include "render/material/ConstantParameter.hpp"

namespace slug::render
{

struct SlotInfo
{
    core::String slotName = "";

    bool operator==(const SlotInfo& slot) const
    {
        return this->slotName == slot.slotName;
    }
};

struct TextureParameter
{
    SlotInfo slot = {};
    TexturePtr texture = nullptr;
};

struct StaticSwitches
{
    core::String name = {};
    bool value = false;
};

class ShaderParameter : public ConstantParameter, public core::SObject
{
public:
    virtual void Reflesh(DrawContext& context);

    virtual void GetTextureSlots(core::TVector<SlotInfo>& slots);
    virtual bool SetTexture(const SlotInfo& slot, TexturePtr texture);
    virtual void AddTexture(const SlotInfo& slot, TexturePtr texture);
    virtual bool GetTexture(const SlotInfo& slot, TexturePtr& texture) const;
    virtual bool HasTexture(const SlotInfo& slot) const;

    virtual void GetStaticSwitches(core::TVector<StaticSwitches>& staticSwiches) const;
    virtual bool SetStaticSwitch(core::StringView name, bool value);
    virtual void AddStaticSwitches(core::StringView name, bool value);
    virtual bool GetStaticSwitch(core::StringView name, bool& value) const;
    virtual bool HasStaticSwitches(core::StringView name) const;

    virtual const rhi::BufferPtr& GetConstantBuffer() const;
    virtual const rhi::BindingLayoutPtr& GetBindingLayout() const;
    virtual const rhi::BindingSetPtr& GetBindingSet() const;


    void SetDrawSetttings(const DrawSettings& settings);
    const DrawSettings& GetDrawSettings() const;

protected:
    void UpdateBindingResource(DrawContext& context, const ConstantParameter& constantParameter);

protected:
    DrawSettings m_drawSettings = {};
    core::TVector<StaticSwitches> m_staticSwitches = {};
    core::TVector<TextureParameter> m_textures = {};
    rhi::BufferPtr m_constantBuffer;
    rhi::BindingLayoutPtr m_bindingLayout;
    rhi::BindingSetPtr m_bindingSet;

};
using ShaderParameterPtr = core::TReferencePtr<ShaderParameter>;

}
