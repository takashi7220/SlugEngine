#pragma once

#include "rhi/ResourceType.hpp"

namespace slug
{
namespace rhi
{

struct CustomSemantic {
    enum class Type
    {
        Undefined = 0,
        XRight = 1,
        ViewportMask = 2
    };

    Type type;
    core::String name;
};

struct ShaderDesc
{
    ShaderType shaderType = ShaderType::None;
    core::String debugName;
    core::String entryName = "main";
    int32_t hlslExtensionsUav = -1;
    bool useSpecificShaderExt = false;
    uint32_t numCustomSemantics = 0;
    CustomSemantic* customSemantics = nullptr;
    FastGeometryShaderFlags fastGeometryFlags = FastGeometryShaderFlags(0);
    uint32_t* coordinateSwizzling = nullptr;
};

struct ShaderSpecialization
{
    uint32_t constantId = 0;
    union
    {
        uint32_t u = 0;
        int32_t i;
        float f;
    } value;

    static ShaderSpecialization CreateUint32(uint32_t _constant_id, uint32_t u)
    {
        ShaderSpecialization s;
        s.constantId = _constant_id;
        s.value.u = u;
        return s;
    }

    static ShaderSpecialization CreateInt32(uint32_t _constant_id, int32_t i)
    {
        ShaderSpecialization s;
        s.constantId = _constant_id;
        s.value.i = i;
        return s;
    }

    static ShaderSpecialization CreateFloat(uint32_t _constant_id, float f)
    {
        ShaderSpecialization s;
        s.constantId = _constant_id;
        s.value.f = f;
        return s;
    }
};

class IShader : public IResource {
public:

    virtual const ShaderDesc& GetDesc() const = 0;
    virtual void GetByteCode(const void** ppByteCode, size_t* pSize) const = 0;
};

using ShaderPtr = core::TReferencePtr<IShader>;

class IShaderLibrary : public IResource
{
public:
    virtual void GetByteCode(const void** ppByteCode, size_t* pSize) const = 0;
    virtual ShaderPtr GetShader(const char* entryName, ShaderType shaderType) = 0;
};

using ShaderLibraryPtr = core::TReferencePtr <IShaderLibrary>;

}
}