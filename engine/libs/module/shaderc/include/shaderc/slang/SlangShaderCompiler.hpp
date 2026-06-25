#pragma once

#include "sscl/ShaderCompilerBase.hpp"
#include "core/memory/UniquePtr.hpp"

namespace slug::sscl
{

class SlangShaderCompiler : public ShaderCompilerBase
{
public:
    SLUG_DECLARE_SOBJECT_TYPEINFO(SlangShaderCompiler, ShaderCompilerBase)

    static core::TReferencePtr<ShaderCompilerBase> Create()
    {
        return core::NewObject<SlangShaderCompiler>();
    }

    SlangShaderCompiler();
    ShaderCompileResult CompileFromFile(const ShaderCompileDesc& desc, const core::StringView sourcePath) override;
    ShaderCompileResult CompileFromSource(const ShaderCompileDesc& desc, const core::String& source) override;

private:
    struct Implement;
    core::TUniquePtr<Implement> m_implement;
};
}
