#include "glm/glm.hpp"
#include "slug_benchmark/Define.hpp"
#include <vector>
#include <benchmark/benchmark.h>

static std::vector<glm::vec3> CreateVector3Array(std::size_t count)
{
    std::vector<glm::vec3> values;
    values.reserve(count);

    for (std::size_t i = 0; i < count; ++i)
    {
        const float f = static_cast<float>(i);

        values.emplace_back(
            f * 0.001f + 1.0f,
            f * 0.002f + 2.0f,
            f * 0.003f + 3.0f);
    }

    return values;
}

SLUG_BENCHMARK_TEST(Vector3Array, Add)
{
    constexpr std::size_t Count = 1024 * 16;

    const std::vector<glm::vec3> a = CreateVector3Array(Count);
    const std::vector<glm::vec3> b = CreateVector3Array(Count);
    std::vector<glm::vec3> result(Count);

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

SLUG_BENCHMARK_TEST(Vector3Array, Sub)
{
    constexpr std::size_t Count = 1024 * 16;

    const std::vector<glm::vec3> a = CreateVector3Array(Count);
    const std::vector<glm::vec3> b = CreateVector3Array(Count);
    std::vector<glm::vec3> result(Count);

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

SLUG_BENCHMARK_TEST(Vector3Array, Mul)
{
    constexpr std::size_t Count = 1024 * 16;

    const std::vector<glm::vec3> a = CreateVector3Array(Count);
    const std::vector<glm::vec3> b = CreateVector3Array(Count);
    std::vector<glm::vec3> result(Count);

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

SLUG_BENCHMARK_TEST(Vector3Array, Div)
{
    constexpr std::size_t Count = 1024 * 16;

    const std::vector<glm::vec3> a = CreateVector3Array(Count);
    const std::vector<glm::vec3> b = CreateVector3Array(Count);
    std::vector<glm::vec3> result(Count);

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

SLUG_BENCHMARK_TEST(Vector3Array, AddScalar)
{
    constexpr std::size_t Count = 1024 * 16;
    constexpr float Scalar = 2.5f;

    const std::vector<glm::vec3> a = CreateVector3Array(Count);
    std::vector<glm::vec3> result(Count);

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

SLUG_BENCHMARK_TEST(Vector3Array, SubScalar)
{
    constexpr std::size_t Count = 1024 * 16;
    constexpr float Scalar = 2.5f;

    const std::vector<glm::vec3> a = CreateVector3Array(Count);
    std::vector<glm::vec3> result(Count);

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

SLUG_BENCHMARK_TEST(Vector3Array, MulScalar)
{
    constexpr std::size_t Count = 1024 * 16;
    constexpr float Scalar = 2.5f;

    const std::vector<glm::vec3> a = CreateVector3Array(Count);
    std::vector<glm::vec3> result(Count);

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

SLUG_BENCHMARK_TEST(Vector3Array, DivScalar)
{
    constexpr std::size_t Count = 1024 * 16;
    constexpr float Scalar = 2.5f;

    const std::vector<glm::vec3> a = CreateVector3Array(Count);
    std::vector<glm::vec3> result(Count);

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

SLUG_BENCHMARK_TEST(Vector3Array, Dot)
{
    constexpr std::size_t Count = 1024 * 16;

    const std::vector<glm::vec3> a = CreateVector3Array(Count);
    const std::vector<glm::vec3> b = CreateVector3Array(Count);

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

SLUG_BENCHMARK_TEST(Vector3Array, Length)
{
    constexpr std::size_t Count = 1024 * 16;

    const std::vector<glm::vec3> a = CreateVector3Array(Count);

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

SLUG_BENCHMARK_TEST(Vector3Array, Normalize)
{
    constexpr std::size_t Count = 1024 * 16;

    const std::vector<glm::vec3> a = CreateVector3Array(Count);
    std::vector<glm::vec3> result(Count);

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

SLUG_BENCHMARK_TEST(Vector3Array, Cross)
{
    constexpr std::size_t Count = 1024 * 16;

    const std::vector<glm::vec3> a = CreateVector3Array(Count);
    const std::vector<glm::vec3> b = CreateVector3Array(Count);
    std::vector<glm::vec3> result(Count);

    for (auto _ : state)
    {
        for (std::size_t i = 0; i < Count; ++i)
        {
            result[i] = glm::cross(a[i], b[i]);
        }

        benchmark::DoNotOptimize(result.data());
        benchmark::ClobberMemory();
    }

    state.SetItemsProcessed(
        static_cast<std::int64_t>(state.iterations()) *
        static_cast<std::int64_t>(Count));
}
