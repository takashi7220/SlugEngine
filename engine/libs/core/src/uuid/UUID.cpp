#pragma once

#include "core/uuid/UUID.hpp"
#include <random>

namespace slug::core
{
UUID::InternalID UUID::GenerateInternal()
{
    std::random_device rd;
    std::mt19937_64 gen(rd());

    UUID::InternalID id;
    id.value[0] = gen();
    id.value[1] = gen();

    id.value[0] &= 0xFFFFFFFFFFFF0FFFULL;
    id.value[0] |= 0x0000000000004000ULL;

    id.value[1] &= 0x3FFFFFFFFFFFFFFFULL;
    id.value[1] |= 0x8000000000000000ULL;

    return id;
}
}
