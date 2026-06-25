#include "core/utility/Hash.hpp"

namespace slug::core
{

uint64_t HashUtility::ConvertStringToHash64(core::StringView str)
{
    return static_cast<uint64_t>(core::StringUtility::Hash(str));
}

void HashUtility::HashCombine(std::size_t& seed, std::size_t value)
{
    seed ^= value + 0x9e3779b97f4a7c15ull + (seed << 12) + (seed >> 4);
}

}
