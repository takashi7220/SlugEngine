#pragma once

#include "core/object/SObject.hpp"

namespace slug::sscl
{

enum class ShaderTargetType
{
    Unknown,
    None,
    GLSL,
    HLSL,
    Metal,
    WGSL,
    SPIRV,
    SPIRV_ASM,
    DXBC,
    DXBC_ASM,
    DXIL,
    DXIL_ASM,
    CUDA_SOURCE,
    CUDA_HEADER,
    PTX,
    METAL_LIB,
    METAL_LIB_ASM,
    WGSL_SPIRV,
    WGSL_SPIRV_ASM,
};

enum class ShaderStageType
{
    None,
    Vertex,
    Hull,
    Domain,
    Geometry,
    Fragment,
    Pixel = Fragment,
    Compute,
    RayGeneration,
    Intersection,
    AnyHit,
    ClosestHit,
    Miss,
    Callable,
    Mesh,
    Amplification,
    Dispatch
};

struct ShaderDefine
{
    core::FixedString<64> name;
    core::FixedString<64> value;
};

struct ShaderCompileDesc
{
    ShaderStageType stageType;
    ShaderTargetType targetType;
    core::FixedString<128> moduleName;
    core::FixedString<64> entryPointName;

    core::FixedString<64> profile;
    core::TVector<core::FixedString<256>> includeDirs;
    core::TVector<ShaderDefine> defines;
};

struct ShaderCompileResult
{
    bool success = false;
    core::TVector<uint8_t> binary;
    core::String message;
};

class ShaderCompilerBase : public core::SObject
{
public:
    SLUG_DECLARE_SOBJECT_TYPEINFO(ShaderCompilerBase, core::SObject)
    virtual ShaderCompileResult CompileFromFile(const ShaderCompileDesc& desc, const core::StringView sourcePath) = 0;
    virtual ShaderCompileResult CompileFromSource(const ShaderCompileDesc& desc, const core::String& source) = 0;

protected:
    ShaderCompilerBase();

};
}
