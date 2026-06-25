#include "math/Matrix4x4.hpp"
#include "slug_benchmark/Define.hpp"
#include <vector>
#include <benchmark/benchmark.h>

static std::vector<slug::math::Matrix4x4> CreateMatrix4x4Array(std::size_t count)
{
    std::vector<slug::math::Matrix4x4> values;
    values.reserve(count);

    for (std::size_t i = 0; i < count; ++i)
    {
        const float f = static_cast<float>(i) * 0.001f;

        values.push_back(slug::math::Matrix4x4(
        {
            { 1.0f + f, f,         f,         f         },
            { f,        1.0f + f,  f,         f         },
            { f,        f,         1.0f + f,  f         },
            { f,        f,         f,         1.0f + f  },
        }));
    }

    return values;
}

static std::vector<slug::math::Vector4> CreateVector4Array(std::size_t count)
{
    std::vector<slug::math::Vector4> values;
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

SLUG_BENCHMARK_TEST(Matrix4x4Array, Add)
{
    constexpr std::size_t Count = 1024 * 16;

    const std::vector<slug::math::Matrix4x4> a = CreateMatrix4x4Array(Count);
    const std::vector<slug::math::Matrix4x4> b = CreateMatrix4x4Array(Count);
    std::vector<slug::math::Matrix4x4> result(Count);

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

SLUG_BENCHMARK_TEST(Matrix4x4Array, Sub)
{
    constexpr std::size_t Count = 1024 * 16;

    const std::vector<slug::math::Matrix4x4> a = CreateMatrix4x4Array(Count);
    const std::vector<slug::math::Matrix4x4> b = CreateMatrix4x4Array(Count);
    std::vector<slug::math::Matrix4x4> result(Count);

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

SLUG_BENCHMARK_TEST(Matrix4x4Array, Mul)
{
    constexpr std::size_t Count = 1024 * 16;

    const std::vector<slug::math::Matrix4x4> a = CreateMatrix4x4Array(Count);
    const std::vector<slug::math::Matrix4x4> b = CreateMatrix4x4Array(Count);
    std::vector<slug::math::Matrix4x4> result(Count);

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

SLUG_BENCHMARK_TEST(Matrix4x4Array, AddScalar)
{
    constexpr std::size_t Count = 1024 * 16;
    constexpr float Scalar = 2.5f;

    const std::vector<slug::math::Matrix4x4> a = CreateMatrix4x4Array(Count);
    std::vector<slug::math::Matrix4x4> result(Count);

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

SLUG_BENCHMARK_TEST(Matrix4x4Array, SubScalar)
{
    constexpr std::size_t Count = 1024 * 16;
    constexpr float Scalar = 2.5f;

    const std::vector<slug::math::Matrix4x4> a = CreateMatrix4x4Array(Count);
    std::vector<slug::math::Matrix4x4> result(Count);

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

SLUG_BENCHMARK_TEST(Matrix4x4Array, MulScalar)
{
    constexpr std::size_t Count = 1024 * 16;
    constexpr float Scalar = 2.5f;

    const std::vector<slug::math::Matrix4x4> a = CreateMatrix4x4Array(Count);
    std::vector<slug::math::Matrix4x4> result(Count);

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

SLUG_BENCHMARK_TEST(Matrix4x4Array, DivScalar)
{
    constexpr std::size_t Count = 1024 * 16;
    constexpr float Scalar = 2.5f;

    const std::vector<slug::math::Matrix4x4> a = CreateMatrix4x4Array(Count);
    std::vector<slug::math::Matrix4x4> result(Count);

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

SLUG_BENCHMARK_TEST(Matrix4x4Array, MulVector)
{
    constexpr std::size_t Count = 1024 * 16;

    const std::vector<slug::math::Matrix4x4> a = CreateMatrix4x4Array(Count);
    std::vector<slug::math::Vector4> vectors(Count);
    for (std::size_t i = 0; i < Count; ++i)
    {
        const float f = static_cast<float>(i);
        vectors[i] = slug::math::Vector4(f * 0.001f + 1.0f, f * 0.002f + 2.0f, f * 0.003f + 3.0f, 1.0f);
    }

    std::vector<slug::math::Vector4> result(Count);

    for (auto _ : state)
    {
        for (std::size_t i = 0; i < Count; ++i)
        {
            result[i] = a[i] * vectors[i];
        }

        benchmark::DoNotOptimize(result.data());
        benchmark::ClobberMemory();
    }

    state.SetItemsProcessed(
        static_cast<std::int64_t>(state.iterations()) *
        static_cast<std::int64_t>(Count));
}

SLUG_BENCHMARK_TEST(Matrix4x4Array, Transpose)
{
    constexpr std::size_t Count = 1024 * 16;

    const std::vector<slug::math::Matrix4x4> a = CreateMatrix4x4Array(Count);
    std::vector<slug::math::Matrix4x4> result(Count);

    for (auto _ : state)
    {
        for (std::size_t i = 0; i < Count; ++i)
        {
            result[i] = a[i].Transpose();
        }

        benchmark::DoNotOptimize(result.data());
        benchmark::ClobberMemory();
    }

    state.SetItemsProcessed(
        static_cast<std::int64_t>(state.iterations()) *
        static_cast<std::int64_t>(Count));
}

SLUG_BENCHMARK_TEST(Matrix4x4Array, Inverse)
{
    constexpr std::size_t Count = 1024 * 16;

    const std::vector<slug::math::Matrix4x4> a = CreateMatrix4x4Array(Count);
    std::vector<slug::math::Matrix4x4> result(Count);

    for (auto _ : state)
    {
        for (std::size_t i = 0; i < Count; ++i)
        {
            result[i] = a[i].Inverse();
        }

        benchmark::DoNotOptimize(result.data());
        benchmark::ClobberMemory();
    }

    state.SetItemsProcessed(
        static_cast<std::int64_t>(state.iterations()) *
        static_cast<std::int64_t>(Count));
}

SLUG_BENCHMARK_TEST(Matrix4x4Array, Determinant)
{
    constexpr std::size_t Count = 1024 * 16;

    const std::vector<slug::math::Matrix4x4> a = CreateMatrix4x4Array(Count);

    float result = 0.0f;

    for (auto _ : state)
    {
        float sum = 0.0f;

        for (std::size_t i = 0; i < Count; ++i)
        {
            sum += a[i].Determinant();
        }

        result = sum;

        benchmark::DoNotOptimize(result);
        benchmark::ClobberMemory();
    }

    state.SetItemsProcessed(
        static_cast<std::int64_t>(state.iterations()) *
        static_cast<std::int64_t>(Count));
}

SLUG_BENCHMARK_TEST(Matrix4x4Array, MulVector4)
{
    constexpr std::size_t Count = 1024 * 16;

    const std::vector<slug::math::Matrix4x4> a = CreateMatrix4x4Array(Count);
    const std::vector<slug::math::Vector4> vectors = CreateVector4Array(Count);
    std::vector<slug::math::Vector4> results(Count);

    float result = 0.0f;

    for (auto _ : state)
    {

        for (std::size_t i = 0; i < Count; ++i)
        {
            results[i] = a[i] * vectors[i];
        }

        result = 0.0f;
        for (std::size_t i = 0; i < Count; ++i)
        {
            result += results[i].x + results[i].y + results[i].z + results[i].w;
        }
        benchmark::DoNotOptimize(result);
        benchmark::ClobberMemory();
    }

    state.SetItemsProcessed(
        static_cast<std::int64_t>(state.iterations()) *
        static_cast<std::int64_t>(Count));
}
