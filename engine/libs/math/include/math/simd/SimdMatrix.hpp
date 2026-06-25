#pragma once

#include "math/simd/SimdVector.hpp"
#include <cmath>

namespace slug::math
{

struct SimdMatrix
{
    SimdVector col[4];
};

namespace simd_matrix
{

inline SimdMatrix Zero()
{
    SimdMatrix m;
    const __m128 zero = _mm_setzero_ps();
    m.col[0] = zero;
    m.col[1] = zero;
    m.col[2] = zero;
    m.col[3] = zero;
    return m;
}

inline SimdMatrix Identity()
{
    SimdMatrix m;
    m.col[0] = _mm_set_ps(0.0f, 0.0f, 0.0f, 1.0f);
    m.col[1] = _mm_set_ps(0.0f, 0.0f, 1.0f, 0.0f);
    m.col[2] = _mm_set_ps(0.0f, 1.0f, 0.0f, 0.0f);
    m.col[3] = _mm_set_ps(1.0f, 0.0f, 0.0f, 0.0f);
    return m;
}

inline void Load(SimdMatrix& m, const float* ptr)
{
    if (ptr == nullptr)
        return;
    m.col[0] = _mm_loadu_ps(ptr + 0);
    m.col[1] = _mm_loadu_ps(ptr + 4);
    m.col[2] = _mm_loadu_ps(ptr + 8);
    m.col[3] = _mm_loadu_ps(ptr + 12);
}

inline void Store(const SimdMatrix& m, float* ptr)
{
    if (ptr == nullptr)
        return;
    _mm_storeu_ps(ptr + 0, m.col[0]);
    _mm_storeu_ps(ptr + 4, m.col[1]);
    _mm_storeu_ps(ptr + 8, m.col[2]);
    _mm_storeu_ps(ptr + 12, m.col[3]);
}

inline bool Equal(const SimdMatrix& m0, const SimdMatrix& m1)
{
    const __m128 eps = _mm_set1_ps(1e-6f);
    const __m128 signMask = _mm_set1_ps(-0.0f);
    for (int i = 0; i < 4; ++i)
    {
        const __m128 absDiff = _mm_andnot_ps(signMask, _mm_sub_ps(m0.col[i], m1.col[i]));
        if (_mm_movemask_ps(_mm_cmple_ps(absDiff, eps)) != 0b1111)
            return false;
    }
    return true;
}

inline SimdMatrix Add(const SimdMatrix& m0, const SimdMatrix& m1)
{
    SimdMatrix r;
    r.col[0] = _mm_add_ps(m0.col[0], m1.col[0]);
    r.col[1] = _mm_add_ps(m0.col[1], m1.col[1]);
    r.col[2] = _mm_add_ps(m0.col[2], m1.col[2]);
    r.col[3] = _mm_add_ps(m0.col[3], m1.col[3]);
    return r;
}

inline SimdMatrix Sub(const SimdMatrix& m0, const SimdMatrix& m1)
{
    SimdMatrix r;
    r.col[0] = _mm_sub_ps(m0.col[0], m1.col[0]);
    r.col[1] = _mm_sub_ps(m0.col[1], m1.col[1]);
    r.col[2] = _mm_sub_ps(m0.col[2], m1.col[2]);
    r.col[3] = _mm_sub_ps(m0.col[3], m1.col[3]);
    return r;
}

inline SimdVector MulVector(const SimdMatrix& m, const SimdVector& v)
{
    const __m128 vx = _mm_shuffle_ps(v, v, _MM_SHUFFLE(0, 0, 0, 0));
    const __m128 vy = _mm_shuffle_ps(v, v, _MM_SHUFFLE(1, 1, 1, 1));
    const __m128 vz = _mm_shuffle_ps(v, v, _MM_SHUFFLE(2, 2, 2, 2));
    const __m128 vw = _mm_shuffle_ps(v, v, _MM_SHUFFLE(3, 3, 3, 3));
    return _mm_add_ps(
        _mm_add_ps(_mm_mul_ps(m.col[0], vx), _mm_mul_ps(m.col[1], vy)),
        _mm_add_ps(_mm_mul_ps(m.col[2], vz), _mm_mul_ps(m.col[3], vw))
    );
}

inline SimdMatrix Mul(const SimdMatrix& m0, const SimdMatrix& m1)
{
    SimdMatrix r;
    r.col[0] = MulVector(m0, m1.col[0]);
    r.col[1] = MulVector(m0, m1.col[1]);
    r.col[2] = MulVector(m0, m1.col[2]);
    r.col[3] = MulVector(m0, m1.col[3]);
    return r;
}

inline SimdMatrix Add(const SimdMatrix& m, float s)
{
    const __m128 scalar = _mm_set1_ps(s);
    SimdMatrix r;
    r.col[0] = _mm_add_ps(m.col[0], scalar);
    r.col[1] = _mm_add_ps(m.col[1], scalar);
    r.col[2] = _mm_add_ps(m.col[2], scalar);
    r.col[3] = _mm_add_ps(m.col[3], scalar);
    return r;
}

inline SimdMatrix Sub(const SimdMatrix& m, float s)
{
    const __m128 scalar = _mm_set1_ps(s);
    SimdMatrix r;
    r.col[0] = _mm_sub_ps(m.col[0], scalar);
    r.col[1] = _mm_sub_ps(m.col[1], scalar);
    r.col[2] = _mm_sub_ps(m.col[2], scalar);
    r.col[3] = _mm_sub_ps(m.col[3], scalar);
    return r;
}

inline SimdMatrix Mul(const SimdMatrix& m, float s)
{
    const __m128 scalar = _mm_set1_ps(s);
    SimdMatrix r;
    r.col[0] = _mm_mul_ps(m.col[0], scalar);
    r.col[1] = _mm_mul_ps(m.col[1], scalar);
    r.col[2] = _mm_mul_ps(m.col[2], scalar);
    r.col[3] = _mm_mul_ps(m.col[3], scalar);
    return r;
}

inline SimdMatrix Div(const SimdMatrix& m, float s)
{
    const __m128 scalar = _mm_set1_ps(s);
    SimdMatrix r;
    r.col[0] = _mm_div_ps(m.col[0], scalar);
    r.col[1] = _mm_div_ps(m.col[1], scalar);
    r.col[2] = _mm_div_ps(m.col[2], scalar);
    r.col[3] = _mm_div_ps(m.col[3], scalar);
    return r;
}

inline SimdMatrix Transpose(const SimdMatrix& m)
{
    const __m128 a = _mm_unpacklo_ps(m.col[0], m.col[1]);
    const __m128 b = _mm_unpacklo_ps(m.col[2], m.col[3]);
    const __m128 c = _mm_unpackhi_ps(m.col[0], m.col[1]);
    const __m128 d = _mm_unpackhi_ps(m.col[2], m.col[3]);
    SimdMatrix r;
    r.col[0] = _mm_movelh_ps(a, b);
    r.col[1] = _mm_movehl_ps(b, a);
    r.col[2] = _mm_movelh_ps(c, d);
    r.col[3] = _mm_movehl_ps(d, c);
    return r;
}

template<int32_t Lane>
inline float GetLane(SimdVector v)
{
    static_assert(0 <= Lane && Lane < 4);

    if constexpr (Lane == 0)
    {
        return _mm_cvtss_f32(v);
    } else
    {
        return _mm_cvtss_f32(_mm_shuffle_ps(v, v, _MM_SHUFFLE(Lane, Lane, Lane, Lane)));
    }
}

template<int32_t Pivot, int32_t Row>
inline void EliminateRow(SimdVector a[4], SimdVector inv[4])
{
    if constexpr (Pivot != Row)
    {
        const SimdVector factor = _mm_set1_ps(GetLane<Pivot>(a[Row]));

        a[Row] = _mm_sub_ps(a[Row], _mm_mul_ps(factor, a[Pivot]));
        inv[Row] = _mm_sub_ps(inv[Row], _mm_mul_ps(factor, inv[Pivot]));
    }
}

template<int32_t Pivot>
inline bool PivotStep(SimdVector a[4], SimdVector inv[4])
{
    const float diag = GetLane<Pivot>(a[Pivot]);

    if (math::TAbs(diag) < 1.0e-8f)
    {
        return false;
    }

    const SimdVector invDiag = _mm_set1_ps(1.0f / diag);

    a[Pivot] = _mm_mul_ps(a[Pivot], invDiag);
    inv[Pivot] = _mm_mul_ps(inv[Pivot], invDiag);

    EliminateRow<Pivot, 0>(a, inv);
    EliminateRow<Pivot, 1>(a, inv);
    EliminateRow<Pivot, 2>(a, inv);
    EliminateRow<Pivot, 3>(a, inv);

    return true;
}

inline SimdMatrix Inverse(const SimdMatrix& m)
{
    SimdVector a[4] =
    {
        m.col[0],
        m.col[1],
        m.col[2],
        m.col[3],
    };

    _MM_TRANSPOSE4_PS(a[0], a[1], a[2], a[3]);

    // Identity matrix as row vectors.
    SimdVector inv[4] =
    {
        _mm_setr_ps(1.0f, 0.0f, 0.0f, 0.0f),
        _mm_setr_ps(0.0f, 1.0f, 0.0f, 0.0f),
        _mm_setr_ps(0.0f, 0.0f, 1.0f, 0.0f),
        _mm_setr_ps(0.0f, 0.0f, 0.0f, 1.0f),
    };

    if (!PivotStep<0>(a, inv)) return Zero();
    if (!PivotStep<1>(a, inv)) return Zero();
    if (!PivotStep<2>(a, inv)) return Zero();
    if (!PivotStep<3>(a, inv)) return Zero();

    // inv[0..3] are rows of inverse matrix.
    // Convert rows back to columns.
    _MM_TRANSPOSE4_PS(inv[0], inv[1], inv[2], inv[3]);

    SimdMatrix result;
    result.col[0] = inv[0];
    result.col[1] = inv[1];
    result.col[2] = inv[2];
    result.col[3] = inv[3];
    return result;
}

inline float Determinant(const SimdMatrix& m)
{
    alignas(16) float e[16];
    Store(m, e);

    const float s0 = e[10] * e[15] - e[14] * e[11];
    const float s1 = e[6] * e[15] - e[14] * e[7];
    const float s2 = e[6] * e[11] - e[10] * e[7];
    const float s3 = e[2] * e[15] - e[14] * e[3];
    const float s4 = e[2] * e[11] - e[10] * e[3];
    const float s5 = e[2] * e[7] - e[6] * e[3];

    return
        e[0] * (e[5] * s0 - e[9] * s1 + e[13] * s2) -
        e[4] * (e[1] * s0 - e[9] * s3 + e[13] * s4) +
        e[8] * (e[1] * s1 - e[5] * s3 + e[13] * s5) -
        e[12] * (e[1] * s2 - e[5] * s4 + e[9] * s5);
}
}
}
