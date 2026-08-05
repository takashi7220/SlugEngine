#pragma once
#include "core/container/String.hpp"
namespace slug::core
{

class HashUtility
{
public:
    static uint64_t CreateHash64();
    static uint64_t CreateHash64(const void* data, size_t size);
    static uint64_t CreateHash64(core::StringView str);
    static void HashCombine(std::size_t& seed, std::size_t value);
};
}
