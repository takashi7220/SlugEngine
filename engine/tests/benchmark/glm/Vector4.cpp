#include "glm/glm.hpp"
#include "slug_benchmark/Define.hpp"
#include <vector>
#include <benchmark/benchmark.h>

static std::vector<glm::vec4> CreateVector4Array(std::size_t count)
{
    std::vector<glm::vec4> values;
    values.reserve(count);

    for (std::size_t i = 0; i < count; ++i)
    {
        const float f = static_cast<float>(i);

        values.emplace_back(
            f * 0.001f + 1.0f,
            f * 0.002f + 2.0f,
            f * 0.003f + 3.0f,
            f * 0.004f + 4.0f);
    }

    return values;
}

SLUG_BENCHMARK_TEST(Vector4Array, Add)
{
    constexpr std::size_t Count = 1024 * 16;

    const std::vector<glm::vec4> a = CreateVector4Array(Count);
    const std::vector<glm::vec4> b = CreateVector4Array(Count);
    std::vector<glm::vec4> result(Count);

    for (auto _ : state)
    {
        for (std::size_t i = 0; i < Count; ++i)
        {
            result[i] = a[i] + b[i];
        }

        benchmark::DoNotOptimize(result.data());
        benchmark::ClobberMemory();
    }

    state.SetItemsProcessed(
        static_cast<std::int64_t>(state.iterations()) *
        static_cast<std::int64_t>(Count));
}

SLUG_BENCHMARK_TEST(Vector4Array, Sub)
{
    constexpr std::size_t Count = 1024 * 16;

    const std::vector<glm::vec4> a = CreateVector4Array(Count);
    const std::vector<glm::vec4> b = CreateVector4Array(Count);
    std::vector<glm::vec4> result(Count);

    for (auto _ : state)
    {
        for (std::size_t i = 0; i < Count; ++i)
        {
            result[i] = a[i] - b[i];
        }

        benchmark::DoNotOptimize(result.data());
        benchmark::ClobberMemory();
    }

    state.SetItemsProcessed(
        static_cast<std::int64_t>(state.iterations()) *
        static_cast<std::int64_t>(Count));
}

SLUG_BENCHMARK_TEST(Vector4Array, Mul)
{
    constexpr std::size_t Count = 1024 * 16;

    const std::vector<glm::vec4> a = CreateVector4Array(Count);
    const std::vector<glm::vec4> b = CreateVector4Array(Count);
    std::vector<glm::vec4> result(Count);

    for (auto _ : state)
    {
        for (std::size_t i = 0; i < Count; ++i)
        {
            result[i] = a[i] * b[i];
        }

        benchmark::DoNotOptimize(result.data());
        benchmark::ClobberMemory();
    }

    state.SetItemsProcessed(
        static_cast<std::int64_t>(state.iterations()) *
        static_cast<std::int64_t>(Count));
}

SLUG_BENCHMARK_TEST(Vector4Array, Div)
{
    constexpr std::size_t Count = 1024 * 16;

    const std::vector<glm::vec4> a = CreateVector4Array(Count);
    const std::vector<glm::vec4> b = CreateVector4Array(Count);
    std::vector<glm::vec4> result(Count);

    for (auto _ : state)
    {
        for (std::size_t i = 0; i < Count; ++i)
        {
            result[i] = a[i] / b[i];
        }

        benchmark::DoNotOptimize(result.data());
        benchmark::ClobberMemory();
    }

    state.SetItemsProcessed(
        static_cast<std::int64_t>(state.iterations()) *
        static_cast<std::int64_t>(Count));
}

SLUG_BENCHMARK_TEST(Vector4Array, AddScalar)
{
    constexpr std::size_t Count = 1024 * 16;
    constexpr float Scalar = 2.5f;

    const std::vector<glm::vec4> a = CreateVector4Array(Count);
    std::vector<glm::vec4> result(Count);

    for (auto _ : state)
    {
        for (std::size_t i = 0; i < Count; ++i)
        {
            result[i] = a[i] + Scalar;
        }

        benchmark::DoNotOptimize(result.data());
        benchmark::ClobberMemory();
    }

    state.SetItemsProcessed(
        static_cast<std::int64_t>(state.iterations()) *
        static_cast<std::int64_t>(Count));
}

SLUG_BENCHMARK_TEST(Vector4Array, SubScalar)
{
    constexpr std::size_t Count = 1024 * 16;
    constexpr float Scalar = 2.5f;

    const std::vector<glm::vec4> a = CreateVector4Array(Count);
    std::vector<glm::vec4> result(Count);

    for (auto _ : state)
    {
        for (std::size_t i = 0; i < Count; ++i)
        {
            result[i] = a[i] - Scalar;
        }

        benchmark::DoNotOptimize(result.data());
        benchmark::ClobberMemory();
    }

    state.SetItemsProcessed(
        static_cast<std::int64_t>(state.iterations()) *
        static_cast<std::int64_t>(Count));
}

SLUG_BENCHMARK_TEST(Vector4Array, MulScalar)
{
    constexpr std::size_t Count = 1024 * 16;
    constexpr float Scalar = 2.5f;

    const std::vector<glm::vec4> a = CreateVector4Array(Count);
    std::vector<glm::vec4> result(Count);

    for (auto _ : state)
    {
        for (std::size_t i = 0; i < Count; ++i)
        {
            result[i] = a[i] * Scalar;
        }

        benchmark::DoNotOptimize(result.data());
        benchmark::ClobberMemory();
    }

    state.SetItemsProcessed(
        static_cast<std::int64_t>(state.iterations()) *
        static_cast<std::int64_t>(Count));
}

SLUG_BENCHMARK_TEST(Vector4Array, DivScalar)
{
    constexpr std::size_t Count = 1024 * 16;
    constexpr float Scalar = 2.5f;

    const std::vector<glm::vec4> a = CreateVector4Array(Count);
    std::vector<glm::vec4> result(Count);

    for (auto _ : state)
    {
        for (std::size_t i = 0; i < Count; ++i)
        {
            result[i] = a[i] / Scalar;
        }

        benchmark::DoNotOptimize(result.data());
        benchmark::ClobberMemory();
    }

    state.SetItemsProcessed(
        static_cast<std::int64_t>(state.iterations()) *
        static_cast<std::int64_t>(Count));
}

SLUG_BENCHMARK_TEST(Vector4Array, Dot)
{
    constexpr std::size_t Count = 1024 * 16;

    const std::vector<glm::vec4> a = CreateVector4Array(Count);
    const std::vector<glm::vec4> b = CreateVector4Array(Count);

    float result = 0.0f;

    for (auto _ : state)
    {
        float sum = 0.0f;

        for (std::size_t i = 0; i < Count; ++i)
        {
            sum += glm::dot(a[i], b[i]);
        }

        result = sum;

        benchmark::DoNotOptimize(result);
        benchmark::ClobberMemory();
    }

    state.SetItemsProcessed(
        static_cast<std::int64_t>(state.iterations()) *
        static_cast<std::int64_t>(Count));
}

SLUG_BENCHMARK_TEST(Vector4Array, Length)
{
    constexpr std::size_t Count = 1024 * 16;

    const std::vector<glm::vec4> a = CreateVector4Array(Count);

    float result = 0.0f;

    for (auto _ : state)
    {
        float sum = 0.0f;

        for (std::size_t i = 0; i < Count; ++i)
        {
            sum += glm::length(a[i]);
        }

        result = sum;

        benchmark::DoNotOptimize(result);
        benchmark::ClobberMemory();
    }

    state.SetItemsProcessed(
        static_cast<std::int64_t>(state.iterations()) *
        static_cast<std::int64_t>(Count));
}

SLUG_BENCHMARK_TEST(Vector4Array, Normalize)
{
    constexpr std::size_t Count = 1024 * 16;

    const std::vector<glm::vec4> a = CreateVector4Array(Count);
    std::vector<glm::vec4> result(Count);

    for (auto _ : state)
    {
        for (std::size_t i = 0; i < Count; ++i)
        {
            result[i] = glm::normalize(a[i]);
        }

        benchmark::DoNotOptimize(result.data());
        benchmark::ClobberMemory();
    }

    state.SetItemsProcessed(
        static_cast<std::int64_t>(state.iterations()) *
        static_cast<std::int64_t>(Count));
}

SLUG_BENCHMARK_TEST(Vector4Array, Cross)
{
    constexpr std::size_t Count = 1024 * 16;

    const std::vector<glm::vec4> a = CreateVector4Array(Count);
    const std::vector<glm::vec4> b = CreateVector4Array(Count);
    std::vector<glm::vec3> result(Count);

    for (auto _ : state)
    {
        for (std::size_t i = 0; i < Count; ++i)
        {
            result[i] = glm::cross(glm::vec3(a[i]), glm::vec3(b[i]));
        }

        benchmark::DoNotOptimize(result.data());
        benchmark::ClobberMemory();
    }

    state.SetItemsProcessed(
        static_cast<std::int64_t>(state.iterations()) *
        static_cast<std::int64_t>(Count));
}
