#pragma once

#include "core/Function.hpp"
#include "rhi/Rhi.hpp"
#include "render/material/DrawSettings.hpp"
#include "render/material/ShaderParameter.hpp"
#include "math/AABB.hpp"

namespace slug::render
{

enum class VertexAttributeType : uint8_t
{
    Position,
    Normal,
    Texcoord,
    Tangent,
    Binormal,
    Color,
    BoneIndex,
    BoneWeight,
    Index,
    Unknown
};

class SlotUtility
{
public:
    static void SetupCallsSlotIndexCallback(core::TFunctionObject<uint32_t(core::StringView)> callback);
    static uint32_t GetSlotIndex(core::StringView name);
    static core::StringView GetSceneSlotName();
    static core::StringView GetViewSlotName();
    static core::StringView GetObjectSlotName();
    static core::StringView GetMaterialSlotName();
    static core::StringView GetVertexAttributeName(VertexAttributeType type);
};

class ShaderVariationUtility
{
public:
    static core::StringView GetDefaultShaderName();
    static core::StringView GetVertexShaderName(bool instancing);
    static core::StringView GetPixelShaderName(core::TVector<StaticSwitches>& staticSwiches, const DrawSettings& drawSettings);
};

class RasterizeUtility
{
public:
    static bool GetFrontCounterClockWise();
    static rhi::ViewportState CreateViewportState(size_t width, size_t height);
};

class ConstantBufferUtility
{
public:
    struct ObjectConstantParameter
    {
        math::Matrix4x4 world;
        math::Matrix4x4 preWorld;
        math::Matrix4x4 worldNormal;
        math::AABB aabb;
    };

    static void UpdateObjectShaderParameter(ShaderParameterPtr& shaderParameter, const ObjectConstantParameter& parameter, bool isUpdate = false);
};

}
