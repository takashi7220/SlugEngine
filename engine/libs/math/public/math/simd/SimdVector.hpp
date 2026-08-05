#pragma once

#include <intrin.h>

namespace slug::math
{
using SimdVector = __m128;

namespace simd_vector
{
inline SimdVector Zero()
{
    return _mm_setzero_ps();
}

inline void Load(SimdVector& v, const float* ptr, size_t count = 4)
{
    if (ptr == nullptr || count > 4)
    {
        return;
    }

    switch (count)
    {
    case 4:
        v = _mm_loadu_ps(ptr);
        break;

    case 3:
        v = _mm_set_ps(0.0f, ptr[2], ptr[1], ptr[0]);
        break;

    case 2:
        v = _mm_set_ps(0.0f, 0.0f, ptr[1], ptr[0]);
        break;

    case 1:
        v = _mm_set_ps(0.0f, 0.0f, 0.0f, ptr[0]);
        break;

    default:
        v = _mm_setzero_ps();
        break;
    }
}

inline void Store(const SimdVector& v, float* ptr, size_t count = 4)
{
    if (ptr == nullptr || count > 4)
    {
        return;
    }

    alignas(16) float temp[4];
    _mm_store_ps(temp, v);

    for (size_t i = 0; i < count; ++i)
    {
        ptr[i] = temp[i];
    }
}

inline bool Equal(const SimdVector& v0, const SimdVector& v1)
{
    const SimdVector diff = _mm_sub_ps(v0, v1);

    const SimdVector signMask = _mm_set1_ps(-0.0f);
    const SimdVector absDiff = _mm_andnot_ps(signMask, diff);

    const SimdVector eps = _mm_set1_ps(1e-6f);
    const SimdVector cmp = _mm_cmple_ps(absDiff, eps);

    return _mm_movemask_ps(cmp) == 0b1111;
}

inline void SetX(SimdVector& v, float x)
{
    alignas(16) float tmp[4];
    Store(v, tmp);
    tmp[0] = x;
    Load(v, tmp);
}

inline void SetY(SimdVector& v, float y)
{
    alignas(16) float tmp[4];
    Store(v, tmp);
    tmp[1] = y;
    Load(v, tmp);
}

inline void SetZ(SimdVector& v, float z)
{
    alignas(16) float tmp[4];
    Store(v, tmp);
    tmp[2] = z;
    Load(v, tmp);
}

inline void SetW(SimdVector& v, float w)
{
    alignas(16) float tmp[4];
    Store(v, tmp);
    tmp[3] = w;
    Load(v, tmp);
}

inline float GetX(const SimdVector& v)
{
    return _mm_cvtss_f32(v);
}

inline float GetY(const SimdVector& v)
{
    return _mm_cvtss_f32(_mm_shuffle_ps(v, v, _MM_SHUFFLE(1, 1, 1, 1)));
}

inline float GetZ(const SimdVector& v)
{
    return _mm_cvtss_f32(_mm_shuffle_ps(v, v, _MM_SHUFFLE(2, 2, 2, 2)));
}

inline float GetW(const SimdVector& v)
{
    return _mm_cvtss_f32(_mm_shuffle_ps(v, v, _MM_SHUFFLE(3, 3, 3, 3)));
}

inline SimdVector Add(const SimdVector& v0, float v1)
{
    return SimdVector(_mm_add_ps(v0, _mm_set1_ps(v1)));
}

inline SimdVector Sub(const SimdVector& v0, float v1)
{
    return SimdVector(_mm_sub_ps(v0, _mm_set1_ps(v1)));
}

inline SimdVector Mul(const SimdVector& v0, float v1)
{
    return SimdVector(_mm_mul_ps(v0, _mm_set1_ps(v1)));
}

inline SimdVector Div(const SimdVector& v0, float v1)
{
    return SimdVector(_mm_div_ps(v0, _mm_set1_ps(v1)));
}

inline SimdVector Add(const SimdVector& v0, const SimdVector& v1)
{
    return SimdVector(_mm_add_ps(v0, v1));
}

inline SimdVector Sub(const SimdVector& v0, const SimdVector& v1)
{
    return SimdVector(_mm_sub_ps(v0, v1));
}

inline SimdVector Mul(const SimdVector& v0, const SimdVector& v1)
{
    return SimdVector(_mm_mul_ps(v0, v1));
}

inline SimdVector Div(const SimdVector& v0, const SimdVector& v1)
{
    return SimdVector(_mm_div_ps(v0, v1));
}

inline SimdVector Cross(const SimdVector& v0, const SimdVector& v1)
{
    const SimdVector a_yzx = _mm_shuffle_ps(v0, v0, _MM_SHUFFLE(3, 0, 2, 1));
    const SimdVector b_zxy = _mm_shuffle_ps(v1, v1, _MM_SHUFFLE(3, 1, 0, 2));

    const SimdVector a_zxy = _mm_shuffle_ps(v0, v0, _MM_SHUFFLE(3, 1, 0, 2));
    const SimdVector b_yzx = _mm_shuffle_ps(v1, v1, _MM_SHUFFLE(3, 0, 2, 1));
    SimdVector result = _mm_sub_ps(
        _mm_mul_ps(a_yzx, b_zxy),
        _mm_mul_ps(a_zxy, b_yzx)
    );

    result = _mm_blend_ps(result, _mm_setzero_ps(), 0b1000);

    return SimdVector(result);
}

inline SimdVector Normalize(const SimdVector& v)
{
    const SimdVector dot = _mm_dp_ps(v, v, 0b11111111);
    const SimdVector length = _mm_sqrt_ps(dot);

    const SimdVector epsilon = _mm_set1_ps(1.0e-8f);
    const SimdVector safeLength = _mm_max_ps(length, epsilon);

    SimdVector result = _mm_div_ps(v, safeLength);

    return SimdVector(result);
}

inline SimdVector NormalizeXYZ(const SimdVector& v)
{
    const SimdVector dot = _mm_dp_ps(v, v, 0b01111111);
    const SimdVector length = _mm_sqrt_ps(dot);

    const SimdVector epsilon = _mm_set1_ps(1.0e-8f);
    const SimdVector safeLength = _mm_max_ps(length, epsilon);

    const SimdVector normalized = _mm_div_ps(v, safeLength);
    const SimdVector result = _mm_blend_ps(normalized, v, 0b1000);

    return SimdVector(result);
}

inline float Dot(const SimdVector& v0, const SimdVector& v1)
{
    const SimdVector dot = _mm_dp_ps(v0, v1, 0b11110001);
    return _mm_cvtss_f32(dot);
}

inline float DotXYZ(const SimdVector& v0, const SimdVector& v1)
{
    const SimdVector dot = _mm_dp_ps(v0, v1, 0b01110001);
    return _mm_cvtss_f32(dot);
}

inline float Length(const SimdVector& v)
{
    const SimdVector dot = _mm_dp_ps(v, v, 0b11110001);
    const SimdVector len = _mm_sqrt_ss(dot);
    return _mm_cvtss_f32(len);
}

inline float LengthXYZ(const SimdVector& v)
{
    const SimdVector dot = _mm_dp_ps(v, v, 0b01110001);
    const SimdVector length = _mm_sqrt_ss(dot);
    return _mm_cvtss_f32(length);
}
}
}
