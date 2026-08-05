#pragma once
#define SLUG_BENCHMARK_TEST(Category, Name)                         \
    static void BM_##Category##_##Name(benchmark::State& state);    \
    BENCHMARK(BM_##Category##_##Name);                              \
    static void BM_##Category##_##Name(benchmark::State& state)     \
