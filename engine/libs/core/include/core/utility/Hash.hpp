#pragma once
#include "core/container/String.hpp"
namespace slug
{
namespace core
{

class HashUtility
{
public:
    static uint64_t ConvertStringToHash64(core::StringView str);
    static void HashCombine(std::size_t& seed, std::size_t value);
};
}
}
