#include "core/utility/Hash.hpp"
#include "core/container/UnorderedMap.hpp"
#include "core/memory/MemoryUtilities.hpp"
namespace slug
{
namespace core
{

uint64_t HashUtility::ConvertStringToHash64(core::StringView str)
{
    const uint64_t fnv_offset_basis = 14695981039346656037ull;
    const uint64_t fnv_prime = 1099511628211ull;
    uint64_t hash = fnv_offset_basis;
    for (char c : str)
    {
        hash ^= static_cast<uint8_t>(c);
        hash *= fnv_prime;
    }
    return hash;
}

void HashUtility::HashCombine(std::size_t& seed, std::size_t value)
{
    seed ^= value + 0x9e3779b97f4a7c15ull + (seed << 12) + (seed >> 4);
}

}
}
