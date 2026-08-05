#include "core/container/String.hpp"
#include "core/utility/Hash.hpp"
#include "slug_benchmark/Define.hpp"
#include <benchmark/benchmark.h>

namespace
{

static slug::core::String CreateDelimitedString(std::size_t count)
{
    slug::core::String text;
    text.reserve(count * 8);

    for (std::size_t i = 0; i < count; ++i)
    {
        text += "token";
        text += slug::core::StringUtility::ToString(static_cast<uint32_t>(i));

        if (i + 1 < count)
        {
            text += ",";
        }
    }

    return text;
}

SLUG_BENCHMARK_TEST(CoreString, Hash)
{
    const slug::core::String text = CreateDelimitedString(1024);
    std::size_t result = 0;

    for (auto _ : state)
    {
        result = slug::core::StringUtility::Hash(text);

        benchmark::DoNotOptimize(result);
        benchmark::ClobberMemory();
    }

    state.SetBytesProcessed(
        static_cast<std::int64_t>(state.iterations()) *
        static_cast<std::int64_t>(text.size()));
}

SLUG_BENCHMARK_TEST(CoreString, HashUtility)
{
    const slug::core::String text = CreateDelimitedString(1024);
    uint64_t result = 0;

    for (auto _ : state)
    {
        result = slug::core::HashUtility::CreateHash64(text);

        benchmark::DoNotOptimize(result);
        benchmark::ClobberMemory();
    }

    state.SetBytesProcessed(
        static_cast<std::int64_t>(state.iterations()) *
        static_cast<std::int64_t>(text.size()));
}

SLUG_BENCHMARK_TEST(CoreString, Split)
{
    constexpr std::size_t Count = 1024;

    const slug::core::String text = CreateDelimitedString(Count);
    const slug::core::String delimiter = ",";

    for (auto _ : state)
    {
        slug::core::TVector<slug::core::String> tokens;
        slug::core::StringUtility::Split(text, delimiter, tokens);

        benchmark::DoNotOptimize(tokens.data());
        benchmark::ClobberMemory();
    }

    state.SetItemsProcessed(
        static_cast<std::int64_t>(state.iterations()) *
        static_cast<std::int64_t>(Count));
}

}
