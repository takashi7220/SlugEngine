#if !defined(SLUG_SHADER_COMPILE)

#pragma once

#include "core/container/String.hpp"
#include "math/Vector2.hpp"
#include "math/Vector3.hpp"
#include "math/Vector4.hpp"
#include "math/Matrix4x4.hpp"

#endif //!defined(SLUG_SHADER_COMPILE)

#define SLUG_SHADER_REGISTER_SCENE_CB 0
#define SLUG_SHADER_REGISTER_VIEW_CB 1
#define SLUG_SHADER_REGISTER_OBJECT_CB 2
#define SLUG_SHADER_REGISTER_MATERIAL_CB 3

#if defined(SLUG_SHADER_COMPILE)

#define SLIUG_SHADER_CONCAT_INTERNAL(a, b) a##b
#define SLIUG_SHADER_CONCAT(a, b) SLIUG_SHADER_CONCAT_INTERNAL(a, b)

#define SLUG_SHADER_BOOL(name) bool name;
#define SLUG_SHADER_FLOAT(name) float name;
#define SLUG_SHADER_FLOAT_ARRAY(name, count) float name[count];
#define SLUG_SHADER_FLOAT2(name) float2 name;
#define SLUG_SHADER_FLOAT3(name) float3 name;
#define SLUG_SHADER_FLOAT4(name) float4 name;
#define SLUG_SHADER_MATRIX4x4(name) float4x4 name;
#define SLUG_CBUFFER(name, id) cbuffer name : register(SLIUG_SHADER_CONCAT(b, id))

#else
#define SLUG_SHADER_BOOL(name) { #name, ParameterType::Bool, 1, sizeof(bool)},
#define SLUG_SHADER_FLOAT(name) { #name, ParameterType::Float, 1, sizeof(float)},
#define SLUG_SHADER_FLOAT_ARRAY(name, count) { #name, ParameterType::FloatArray, count, sizeof(float) * count},
#define SLUG_SHADER_FLOAT2(name) { #name, ParameterType::Float2, 1, sizeof(math::Vector2)},
#define SLUG_SHADER_FLOAT3(name) { #name, ParameterType::Float3, 1, sizeof(math::Vector3)},
#define SLUG_SHADER_FLOAT4(name) { #name, ParameterType::Float4, 1, sizeof(math::Vector4)},
#define SLUG_SHADER_MATRIX4x4(name) { #name, ParameterType::Float4x4, 1, sizeof(math::Matrix4x4)},
#define SLUG_CBUFFER(name, id) ShaderParameter name[] = 

namespace slug
{
namespace shader
{

enum class ParameterType : uint8_t
{
    Bool,
    Float,
    Float2,
    Float3,
    Float4,
    Float3x3,
    Float4x4,
    FloatArray,
};

struct ShaderParameter
{
    core::FixedString<128> name;
    ParameterType type;
    size_t count;
    size_t size;
};
}
}
#endif //defined(SLUG_SHADER_COMPILE)
