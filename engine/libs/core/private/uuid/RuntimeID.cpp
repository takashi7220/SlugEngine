#include "core/uuid/RuntimeID.hpp"

#include <cstdlib>

#include "core/NumericLimits.hpp"
#include "core/debug/Assert.hpp"
#include "core/thread/Atomic.hpp"

namespace slug::core
{

RuntimeID RuntimeID::Generate() noexcept
{
    RuntimeID id;
    if (!TryGenerate(id))
    {
        SLUG_ASSERT_MSG(false, "RuntimeID space exhausted");
        std::abort();
    }
    return id;
}

bool RuntimeID::TryGenerate(RuntimeID& outID) noexcept
{
    // 全利用箇所で採番元を共有し、上限に達しても周回・再利用させない。
    static TAtomic<uint64_t> counter { 0 };
    uint64_t current = counter.load(MemoryOrderRelaxed);
    for (;;)
    {
        if (current == TNumericLimits<uint64_t>::max())
        {
            return false;
        }
        if (counter.compare_exchange_weak(current, current + 1, MemoryOrderRelaxed))
        {
            outID = RuntimeID(current + 1);
            return true;
        }
    }
}

}
