#pragma once

#include "core/container/String.hpp"
#include "core/container/Vector.hpp"
#include "sscl/CommandOptions.hpp"

namespace slug::sscl
{
class ShaderCompiler
{
public:
    bool Compile(int argc, char** argv);
    bool Compile(const core::TVector<core::String>& args);
    bool Compile(const CommandOptions& options);
};
}
