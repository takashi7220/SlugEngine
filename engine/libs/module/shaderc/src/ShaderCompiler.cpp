#include "sscl/ShaderCompiler.hpp"
#include "sscl/ShaderCompilerBase.hpp"
#include "sscl/slang/SlangShaderCompiler.hpp"

namespace slug::sscl
{

bool ShaderCompiler::Compile(int argc, char** argv)
{
    CommandOptions options;
    core::TCommandParser<CommandOptions> cmdLine(options);
    core::CommandParser::ParseResult result = cmdLine.Parse(argc, argv);
    if (result.resultType == core::CommandParser::ResultType::Success)
    {
        return Compile(options);
    }
    return false;
}

bool ShaderCompiler::Compile(const core::TVector<core::String>& args)
{
    CommandOptions options;
    core::TCommandParser<CommandOptions> cmdLine(options);
    core::CommandParser::ParseResult result = cmdLine.Parse(args);
    if (result.resultType == core::CommandParser::ResultType::Success)
    {
        return Compile(options);
    }
    return false;
}

bool ShaderCompiler::Compile(const CommandOptions& options)
{
    core::TReferencePtr<ShaderCompilerBase> compiler = SlangShaderCompiler::Create();
}

}
