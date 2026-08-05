#include "sscl/slang/SlangShaderCompiler.hpp"
#include "core/filesystem/FileSystem.hpp"
#include <slang/slang.h>
#include <slang/slang-com-ptr.h>

namespace slug::sscl
{

namespace utility
{
struct SlangStageConvertInfo
{
    ShaderStageType ssclStage;
    SlangStage slangStage;
};

struct SlangTargetConvertInfo
{
    ShaderTargetType ssclTarget;
    SlangCompileTarget slangTarget;
};

static const SlangStageConvertInfo s_slangShaderStageConvertInfos[] =
{
    {ShaderStageType::None,          SlangStage::SLANG_STAGE_NONE},
    {ShaderStageType::Vertex,        SlangStage::SLANG_STAGE_VERTEX},
    {ShaderStageType::Hull,          SlangStage::SLANG_STAGE_HULL},
    {ShaderStageType::Domain,        SlangStage::SLANG_STAGE_DOMAIN},
    {ShaderStageType::Geometry,      SlangStage::SLANG_STAGE_GEOMETRY},
    {ShaderStageType::Fragment,      SlangStage::SLANG_STAGE_FRAGMENT},
    {ShaderStageType::Pixel,         SlangStage::SLANG_STAGE_PIXEL},
    {ShaderStageType::Compute,       SlangStage::SLANG_STAGE_COMPUTE},
    {ShaderStageType::RayGeneration, SlangStage::SLANG_STAGE_RAY_GENERATION},
    {ShaderStageType::Intersection,  SlangStage::SLANG_STAGE_INTERSECTION},
    {ShaderStageType::AnyHit,        SlangStage::SLANG_STAGE_ANY_HIT},
    {ShaderStageType::ClosestHit,    SlangStage::SLANG_STAGE_CLOSEST_HIT},
    {ShaderStageType::Miss,          SlangStage::SLANG_STAGE_MISS},
    {ShaderStageType::Callable,      SlangStage::SLANG_STAGE_CALLABLE},
    {ShaderStageType::Amplification, SlangStage::SLANG_STAGE_AMPLIFICATION},
    {ShaderStageType::Dispatch,      SlangStage::SLANG_STAGE_DISPATCH},
};

static const SlangTargetConvertInfo s_slangTargetConvertInfos[] =
{
    {ShaderTargetType::Unknown,         SlangCompileTarget::SLANG_TARGET_UNKNOWN},
    {ShaderTargetType::None,            SlangCompileTarget::SLANG_TARGET_NONE},
    {ShaderTargetType::GLSL,            SlangCompileTarget::SLANG_GLSL},
    {ShaderTargetType::HLSL,            SlangCompileTarget::SLANG_HLSL},
    {ShaderTargetType::Metal,           SlangCompileTarget::SLANG_METAL},
    {ShaderTargetType::WGSL,            SlangCompileTarget::SLANG_WGSL},
    {ShaderTargetType::SPIRV,           SlangCompileTarget::SLANG_SPIRV},
    {ShaderTargetType::SPIRV_ASM,       SlangCompileTarget::SLANG_SPIRV_ASM},
    {ShaderTargetType::DXBC,            SlangCompileTarget::SLANG_DXBC},
    {ShaderTargetType::DXBC_ASM,        SlangCompileTarget::SLANG_DXBC_ASM},
    {ShaderTargetType::DXIL,            SlangCompileTarget::SLANG_DXIL},
    {ShaderTargetType::DXIL_ASM,        SlangCompileTarget::SLANG_DXIL_ASM},
    {ShaderTargetType::CUDA_SOURCE,     SlangCompileTarget::SLANG_CUDA_SOURCE},
    {ShaderTargetType::CUDA_HEADER,     SlangCompileTarget::SLANG_CUDA_HEADER},
    {ShaderTargetType::PTX,             SlangCompileTarget::SLANG_PTX},
    {ShaderTargetType::METAL_LIB,       SlangCompileTarget::SLANG_METAL_LIB},
    {ShaderTargetType::METAL_LIB_ASM,   SlangCompileTarget::SLANG_METAL_LIB_ASM},
    {ShaderTargetType::WGSL_SPIRV,      SlangCompileTarget::SLANG_SPIRV},
    {ShaderTargetType::WGSL_SPIRV_ASM,  SlangCompileTarget::SLANG_SPIRV_ASM}
};


SlangStage ConvertShaderStage(ShaderStageType stage)
{
    for (auto& itr : s_slangShaderStageConvertInfos)
    {
        if (itr.ssclStage == stage)
        {
            return itr.slangStage;
        }
    }
    return SLANG_STAGE_NONE;
}

SlangCompileTarget ConvertShdaerTarget(ShaderTargetType target)
{
    for (auto& itr : s_slangTargetConvertInfos)
    {
        if (itr.ssclTarget == target)
        {
            return itr.slangTarget;
        }
    }
    return SlangCompileTarget::SLANG_TARGET_UNKNOWN;
}

static void AppendDiagnostics(core::String& dst, const Slang::ComPtr<slang::IBlob>& diagnosticsBlob)
{
    if (diagnosticsBlob)
    {
        const char* text = static_cast<const char*>(diagnosticsBlob->getBufferPointer());
        if (text)
        {
            dst += text;
            if (!dst.empty() && dst.back() != '\n')
            {
                dst += '\n';
            }
        }
    }
}

}

struct SlangShaderCompiler::Implement
{
    Slang::ComPtr<slang::IGlobalSession> m_globalSession;
};

SlangShaderCompiler::SlangShaderCompiler()
    : m_implement(core::NewObject<Implement>())
{
    SlangResult result = slang::createGlobalSession(m_implement->m_globalSession.writeRef());
    if (SLANG_FAILED(result))
    {
        SLUG_THROW_EXCEPTION("Failed to create Slang global session");
    }
}

ShaderCompileResult SlangShaderCompiler::CompileFromFile(const ShaderCompileDesc& desc, const core::StringView sourcePath)
{
    core::String source = core::FileSystem::ReadText(sourcePath);
    return CompileFromSource(desc, source);
}

ShaderCompileResult SlangShaderCompiler::CompileFromSource(const ShaderCompileDesc& desc, const core::String& source) 
{
    core::TVector<const char*> searchPaths;
    searchPaths.reserve(desc.includeDirs.size());
    for (const auto& itr : desc.includeDirs)
    {
        searchPaths.push_back(itr.c_str());
    }

    core::TVector<slang::PreprocessorMacroDesc> macros;
    macros.reserve(desc.defines.size());
    for (const auto& def : desc.defines)
    {
        slang::PreprocessorMacroDesc macro = {};
        macro.name = def.name.c_str();
        macro.value = def.value.c_str();
        macros.push_back(macro);
    }

    slang::TargetDesc targetDesc = {};
    {
        targetDesc.format = utility::ConvertShdaerTarget(desc.targetType);
        targetDesc.profile = m_implement->m_globalSession->findProfile(desc.profile.c_str());
    }

    slang::SessionDesc sessionDesc = {};
    {
        sessionDesc.targets = &targetDesc;
        sessionDesc.targetCount = 1;
        sessionDesc.searchPaths = searchPaths.data();
        sessionDesc.searchPathCount = static_cast<SlangInt>(searchPaths.size());
        sessionDesc.preprocessorMacros = macros.data();
        sessionDesc.preprocessorMacroCount = static_cast<SlangInt>(macros.size());
    }

    ShaderCompileResult result = {};

    // Create Session
    Slang::ComPtr<slang::ISession> session;
    {
        SlangResult r = m_implement->m_globalSession->createSession(sessionDesc, session.writeRef());
        if (SLANG_FAILED(r) || !session)
        {
            result.message = "Failed to create Slang session.";
            return result;
        }
    }

    // Load Module
    Slang::ComPtr<slang::IModule> module;
    {
        Slang::ComPtr<slang::IBlob> diagnosticsBlob;
        module = session->loadModuleFromSourceString(desc.moduleName.c_str(), source.c_str(), source.c_str(), diagnosticsBlob.writeRef());
        utility::AppendDiagnostics(result.message, diagnosticsBlob);
        if (!module)
        {
            if (result.message.empty())
            {
                result.message = "Failed to load Slang module.";
            }
            return result;
        }
    }

    // Find EntriyPoint
    Slang::ComPtr<slang::IEntryPoint> entryPoint;
    module->findEntryPointByName(desc.entryPointName.c_str(), entryPoint.writeRef());
    if (!entryPoint)
    {
        core::StringUtility::Snprintf(result.message, "Failed to find entry point: %s \n Make sure the function has [shader(\"compute\")], [shader(\"vertex\")], or [shader(\"fragment\")].\n", desc.entryPointName.c_str());
        return result;
    }

    // Create Component
    slang::IComponentType* components[] = { module, entryPoint};
    Slang::ComPtr<slang::IComponentType> composedProgram;
    {
        Slang::ComPtr<slang::IBlob> diagnosticsBlob;
        SlangResult r = session->createCompositeComponentType(components, 2, composedProgram.writeRef(), diagnosticsBlob.writeRef());
        utility::AppendDiagnostics(result.message, diagnosticsBlob);
        if (SLANG_FAILED(r) || !composedProgram)
        {
            result.message += "Failed to create composite component type.\n";
            return result;
        }
    }

    // Link Program
    Slang::ComPtr<slang::IComponentType> linkedProgram;
    {
        Slang::ComPtr<slang::IBlob> diagnosticsBlob;
        SlangResult r = composedProgram->link(linkedProgram.writeRef(), diagnosticsBlob.writeRef());
        utility::AppendDiagnostics(result.message, diagnosticsBlob);
        if (SLANG_FAILED(r) || !linkedProgram)
        {
            result.message += "Failed to link Slang program.\n";
            return result;
        }
    }

    // Get Code
    Slang::ComPtr<slang::IBlob> codeBlob;
    {
        Slang::ComPtr<slang::IBlob> diagnosticsBlob;
        SlangResult r = linkedProgram->getEntryPointCode(0, 0, codeBlob.writeRef(), diagnosticsBlob.writeRef());
        utility::AppendDiagnostics(result.message, diagnosticsBlob);
        if (SLANG_FAILED(r) || !codeBlob)
        {
            result.message += "Failed to get entry point code.\n";
            return result;
        }
    }

    // Insert Binary
    const void* ptr = codeBlob->getBufferPointer();
    const size_t size = codeBlob->getBufferSize();
    result.binary.resize(size);
    core::MemoryUtility::Memcpy(result.binary.data(), size, ptr, size);
    result.success = true;
    return result;
}
}
