#include "rhi/d3d12/resource/Shader.hpp"

namespace slug
{
namespace rhi
{
namespace d3d12
{

const ShaderDesc& Shader::GetDesc() const
{
    return desc;
}

void Shader::GetByteCode(const void** pp_byte_code, size_t* p_size) const
{
    if (pp_byte_code)
    {
        *pp_byte_code = bytecode.data();
    }

    if (p_size)
    {
        *p_size = bytecode.size();
    }
}

void ShaderLibrary::GetByteCode(const void** ppByteCode, size_t* pSize) const
{
    if (ppByteCode)
    {
        *ppByteCode = bytecode.data();
    }

    if (pSize)
    {
        *pSize = bytecode.size();
    }

}

ShaderPtr ShaderLibrary::GetShader(const char* entryName, ShaderType shaderType)
{
    return ShaderPtr::create(new ShaderLibraryEntry(this, entryName, shaderType));
}

ShaderLibraryEntry::ShaderLibraryEntry(IShaderLibrary* pLibrary, const char* entryName, ShaderType shaderType)
    : library(pLibrary)

{
    desc.shaderType = shaderType;
    desc.entryName = entryName;
}

const ShaderDesc& ShaderLibraryEntry::GetDesc() const
{
    return desc;
}

void ShaderLibraryEntry::GetByteCode(const void** pp_bytecode, size_t* p_size) const
{
    library->GetByteCode(pp_bytecode, p_size);
}

}
}
}
