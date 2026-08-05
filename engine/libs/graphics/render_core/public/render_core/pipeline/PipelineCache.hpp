#pragma once

#include "core/container/String.hpp"

namespace slug::render_core
{

struct PipelineCache
{
    uint64_t key = 0;
    core::TVector<uint8_t> data = {};
    core::FixedString<32> name = {};
};

}
