#pragma once

#include "rhi/interface/IShader.hpp"
#include "rhi/common/StateTracking.hpp"
#include "rhi/d3d12/resource/DeviceResources.hpp"
#include "rhi/ResourceType.hpp"

namespace slug::rhi::d3d12
{

class Shader : public IShader
{
public:
    const ShaderDesc& GetDesc() const override;
    void GetByteCode(const void** pp_byte_code, size_t* p_size) const override;

public:
    ShaderDesc desc;
    core::TVector<char> bytecode;
};

class ShaderLibrary : public IShaderLibrary
{
public:
    core::TVector<char> bytecode;

    void GetByteCode(const void** ppByteCode, size_t* pSize) const override;
    ShaderPtr GetShader(const char* entryName, ShaderType shaderType) override;
};

class ShaderLibraryEntry : public IShader
{
public:
    ShaderLibraryEntry(IShaderLibrary* pLibrary, const char* entryName, ShaderType shaderType);

    const ShaderDesc& GetDesc() const override;
    void GetByteCode(const void** pp_bytecode, size_t* p_size) const override;
public:
    ShaderDesc desc;
    core::TReferencePtr<IShaderLibrary> library;
};

}
