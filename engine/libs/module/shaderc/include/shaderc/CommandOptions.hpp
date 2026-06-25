#pragma once

#include "core/command/CommandParser.hpp"

namespace slug::sscl
{

struct CommandOptions : core::TCommandOptionsBase<CommandOptions>
{
    core::String inputFile = {};
    core::String outputDir = {};
    core::String target = {};
    core::String shaderModel = {};
    core::TVector<core::String> defines = {};
    core::TVector<core::String> includeDirs = {};

    CommandOptions()
    {
        Initialize();
    }

    void RegisterDescriptorAll() override
    {
        SLUG_COMMAND_OPTIONS_REGISTER(CommandOptions, slug::core::String, inputFile, "input_file", 'i', true, true, false, "input file path")
        SLUG_COMMAND_OPTIONS_REGISTER(CommandOptions, slug::core::String, outputDir, "output_dir", 'o', false, true, false, "Output directory path")
        SLUG_COMMAND_OPTIONS_REGISTER(CommandOptions, slug::core::String, target, "target", 't', false, true, false, "Output Target (ex. HLSL / GLSL / DXIL etc...)")
        SLUG_COMMAND_OPTIONS_REGISTER(CommandOptions, slug::core::String, shaderModel, "shader_model", 'm', false, true, false, "Shader Models (ex. sm_6_6 etc...")
        SLUG_COMMAND_OPTIONS_REGISTER(CommandOptions, slug::core::TVector<slug::core::String>, defines, "defines", 'D', false, true, true, "Define Macros")
        SLUG_COMMAND_OPTIONS_REGISTER(CommandOptions, slug::core::TVector<slug::core::String>, includeDirs, "include_dirs", 'I', false, true, true, "Include directories for additional resources")
    }
};

}
