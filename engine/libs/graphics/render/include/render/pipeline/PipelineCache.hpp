#pragma once

#include "core/container/String.hpp"

namespace slug::render
{

struct PipelineCache
{
    uint64_t key;
    core::TVector<uint8_t> data;
    core::FixedString<32> name;
};

}
