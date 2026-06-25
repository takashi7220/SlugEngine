#include "core/memory/ReferencePtr.hpp"
#include "slug_benchmark/Define.hpp"
#include <benchmark/benchmark.h>

namespace
{

struct BenchmarkReferenceObject final : public slug::core::ReferenceObject
{
    int value = 0;

    BenchmarkReferenceObject() = default;

    explicit BenchmarkReferenceObject(int v)
        : value(v)
    {
    }
};

SLUG_BENCHMARK_TEST(CoreReferencePtr, MakeReference)
{
    for (auto _ : state)
    {
        slug::core::TReferencePtr<BenchmarkReferenceObject> value =
            slug::core::MakeReference<BenchmarkReferenceObject>(42);

        benchmark::DoNotOptimize(value.get());
        benchmark::ClobberMemory();
    }

    state.SetItemsProcessed(state.iterations());
}

SLUG_BENCHMARK_TEST(CoreReferencePtr, Copy)
{
    slug::core::TReferencePtr<BenchmarkReferenceObject> source =
        slug::core::MakeReference<BenchmarkReferenceObject>(42);

    for (auto _ : state)
    {
        slug::core::TReferencePtr<BenchmarkReferenceObject> copied = source;

        benchmark::DoNotOptimize(copied.get());
        benchmark::ClobberMemory();
    }

    state.SetItemsProcessed(state.iterations());
}

}
