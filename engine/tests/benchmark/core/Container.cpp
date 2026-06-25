#include "core/container/Vector.hpp"
#include "slug_benchmark/Define.hpp"
#include <benchmark/benchmark.h>

namespace
{

SLUG_BENCHMARK_TEST(CoreVector, PushBackInt)
{
    constexpr std::size_t Count = 1024 * 16;

    for (auto _ : state)
    {
        slug::core::TVector<int> values;
        values.reserve(Count);

        for (std::size_t i = 0; i < Count; ++i)
        {
            values.push_back(static_cast<int>(i));
        }

        benchmark::DoNotOptimize(values.data());
        benchmark::ClobberMemory();
    }

    state.SetItemsProcessed(
        static_cast<std::int64_t>(state.iterations()) *
        static_cast<std::int64_t>(Count));
}

SLUG_BENCHMARK_TEST(CoreVector, IterateInt)
{
    constexpr std::size_t Count = 1024 * 16;

    slug::core::TVector<int> values;
    values.reserve(Count);

    for (std::size_t i = 0; i < Count; ++i)
    {
        values.push_back(static_cast<int>(i));
    }

    int result = 0;

    for (auto _ : state)
    {
        int sum = 0;

        for (int value : values)
        {
            sum += value;
        }

        result = sum;

        benchmark::DoNotOptimize(result);
        benchmark::ClobberMemory();
    }

    state.SetItemsProcessed(
        static_cast<std::int64_t>(state.iterations()) *
        static_cast<std::int64_t>(Count));
}

}
