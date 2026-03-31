
#include "shader/Define.h"
#include "render/utility/Utility.hpp"

namespace slug
{
namespace render
{

static core::TFunctionObject<uint32_t(core::StringView)> CalcSlotIndexCallback = nullptr;

void SlotUtility::SetupCallsSlotIndexCallback(core::TFunctionObject<uint32_t(core::StringView)> callback)
{
    CalcSlotIndexCallback = callback;
}

uint32_t SlotUtility::GetSlotIndex(core::StringView name)
{
    if (CalcSlotIndexCallback != nullptr)
    {
        return CalcSlotIndexCallback(name);
    }
    else
    {
        if (name == "scene")
        {
            return SLUG_SHADER_REGISTER_SCENE_CB;
        }
        else if (name == "view")
        {
            return SLUG_SHADER_REGISTER_VIEW_CB;
        }
        else if (name == "object")
        {
            return SLUG_SHADER_REGISTER_OBJECT_CB;
        }
        else if (name == "material")
        {
            return SLUG_SHADER_REGISTER_MATERIAL_CB;
        }
    }
    return 0;
}

core::StringView SlotUtility::GetSceneSlotName()
{
    return "scene";
}

core::StringView SlotUtility::GetViewSlotName()
{
    return "view";
}

core::StringView SlotUtility::GetObjectSlotName()
{
    return "object";
}

core::StringView SlotUtility::GetMaterialSlotName()
{
    return "material";
}

core::StringView SlotUtility::GetVertexAttributeName(VertexAttributeType type)
{
    switch (type)
    {
    case VertexAttributeType::Position:
        return "POSITION";
    case VertexAttributeType::Normal:
        return "NORMAL";
    case VertexAttributeType::Texcoord:
        return "TEXCOORD";
    case VertexAttributeType::Tangent:
        return "TANGENT";
    case VertexAttributeType::Binormal:
        return "BINORMAL";
    case VertexAttributeType::Color:
        return "COLOR";
    case VertexAttributeType::BoneIndex:
        return "BONEINDEX";
    case VertexAttributeType::BoneWeight:
        return "BONEWIGHE";
    case VertexAttributeType::Index:
        return "INDEX";
    default:
        return "UNKNOWN";
    }
}

core::StringView ShaderVariationUtility::GetDefaultShaderName()
{
    return "default";
}

core::StringView ShaderVariationUtility::GetVertexShaderName(bool instancing)
{
    if (instancing)
    {
        return "instancing";
    }
    else
    {
        return "default";
    }
}

core::StringView ShaderVariationUtility::GetPixelShaderName(core::TVector<StaticSwitches>& staticSwiches, const DrawSettings& drawSettings)
{
    core::StringStream ss = {};
    if (drawSettings.blendOptions == BlendOptions::Opaque)
    {
        ss << "opaque";
    }
    else if (drawSettings.blendOptions == BlendOptions::Mask)
    {
        ss << "mask";
    }
    else
    {
        ss << "transparent";
    }
    ss << "_";

    for (auto& staticSwitch : staticSwiches)
    {
        if (staticSwitch.value)
        {
            ss << staticSwitch.name;
            ss << "_";
        }
    }

    return ss.str().c_str();
}

bool RasterizeUtility::GetFrontCounterClockWise()
{
    return true;
}

rhi::ViewportState RasterizeUtility::CreateViewportState(size_t width, size_t height)
{
    rhi::Viewport viewport((float)width, (float)height);
    rhi::Rect scissorRect(viewport);
    rhi::ViewportState state;
    state.viewports.push_back(viewport);
    state.scissorRects.push_back(scissorRect);
    return state;
}

void ConstantBufferUtility::UpdateObjectShaderParameter(ShaderParameterPtr& shaderParameter, const ObjectConstantParameter& parameter, bool isUpdate)
{
    if (!isUpdate)
    {
        shaderParameter->AddParameter("world", parameter.world);
        shaderParameter->AddParameter("preWorld", parameter.preWorld);
        shaderParameter->AddParameter("worldNormal", parameter.worldNormal);
        shaderParameter->AddParameter("aabb_max", parameter.aabb.max);
        shaderParameter->AddParameter("aabb_min", parameter.aabb.min);
    }
    else
    {
        shaderParameter->SetParameter("world", parameter.world);
        shaderParameter->SetParameter("preWorld", parameter.preWorld);
        shaderParameter->SetParameter("worldNormal", parameter.worldNormal);
        shaderParameter->SetParameter("aabb_max", parameter.aabb.max);
        shaderParameter->SetParameter("aabb_min", parameter.aabb.min);
    }
}

}
}
