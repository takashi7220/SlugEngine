#include "core/utility/Hash.hpp"
#include "core/thread/Atomic.hpp"
#include "core/timer/Timer.hpp"

namespace slug::core
{

namespace hash_internal
{
uint64_t CreateHash64()
{
    static const uint64_t processSeed = static_cast<uint64_t>(core::Timer::GetNowNanoseconds());
    static core::TAtomic<uint64_t> counter { 0 };
    const uint64_t index = counter.fetch_add(1, std::memory_order_relaxed);
    return (processSeed << 16) | (index & 0xFFFFull);
}
}

uint64_t HashUtility::CreateHash64()
{
    return hash_internal::CreateHash64();
}

uint64_t HashUtility::CreateHash64(core::StringView str)
{
    return static_cast<uint64_t>(core::StringUtility::Hash(str));
}

uint64_t HashUtility::CreateHash64(const void* data, size_t size)
{
    uint64_t hash = 0;
    if (data != nullptr && size != 0)
    {
        constexpr uint64_t OffsetBasis = 14695981039346656037ull;
        constexpr uint64_t Prime = 1099511628211ull;

        hash = OffsetBasis;

        const auto* bytes = static_cast<const uint8_t*>(data);

        for (size_t i = 0; i < size; ++i)
        {
            hash ^= static_cast<uint64_t>(bytes[i]);
            hash *= Prime;
        }
    }
    else
    {
        hash = hash_internal::CreateHash64();
    }
    return hash;
}

void HashUtility::HashCombine(std::size_t& seed, std::size_t value)
{
    seed ^= value + 0x9e3779b97f4a7c15ull + (seed << 12) + (seed >> 4);
}

}
