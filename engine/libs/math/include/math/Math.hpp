#pragma once

#include <cmath>
#include <cfloat>
#include <initializer_list>
#include "math/TypeConcept.hpp"


namespace slug::math
{
static constexpr float SLUG_F32_MIN = FLT_MIN;
static constexpr double SLUG_F64_MIN = DBL_MIN;
static constexpr long double SLUG_F128_MIN = LDBL_MIN;

static constexpr float SLUG_F32_EPSILON = FLT_EPSILON;
static constexpr double SLUG_F64_EPSILON = DBL_EPSILON;
static constexpr long double SLUG_F128_EPSILON = LDBL_EPSILON;

static constexpr float SLUG_F32_PI = 3.14159265f;
static constexpr double SLUG_F64_PI = 3.141592653589793;

using ValueType = float;

template<NumericType T>
inline T TMax(T a, T b)
{
    return a < b ? b : a;
}

template<NumericType T>
inline T TMin(T a, T b)
{
    return a < b ? a : b;
}

template<NumericType T>
inline T TAbs(T a)
{
    return a < static_cast<T>(0) ? -a : a;
}

template<NumericType T>
inline T TSqrt(T a)
{
    long double ret = std::sqrtl(static_cast<long double>(a));
    return static_cast<T>(ret);
}

template<NumericType T>
inline T TClamp(T a, T minv, T maxv)
{
    return TMax(TMin(a, maxv), minv);
}

template<NumericType T>
inline T TSaturate(T a)
{
    return TMax(TMin(a, static_cast<T>(1)), static_cast<T>(0));
}

template<NumericType T, FloatingType U>
inline U TLerp(T a, T b, U r)
{
    U rate0 = TSaturate(r);
    U rate1 = (static_cast<U>(1) - rate0);
    return rate1 * static_cast<U>(a) + rate0 * static_cast<U>(b);
}

template<NumericType T>
inline T Rad2Deg(T rad)
{
    T deg = rad * static_cast<T>(180);
    return deg / static_cast<T>(SLUG_F64_PI);
}

template<NumericType T>
inline T Deg2Rad(T deg)
{
    T rad = deg / static_cast<T>(180);
    return rad * static_cast<T>(SLUG_F64_PI);
}

template<NumericType T>
inline T Cos(T radians)
{
    return static_cast<T>(std::cos(static_cast<double>(radians)));
}

template<NumericType T>
inline T Sin(T radians)
{
    return static_cast<T>(std::sin(static_cast<double>(radians)));
}

template<NumericType T>
inline T Tan(T radians)
{
    return static_cast<T>(std::tan(static_cast<double>(radians)));
}

template<FloatingType T>
inline T TPow(T value, T rate)
{
    return static_cast<T>(std::pow(value, rate));
}

template<>
inline float TPow(float value, float rate)
{
    return std::powf(value, rate);
}

template<>
inline long double TPow(long double value, long double rate)
{
    return std::powl(value, rate);
}

template<FloatingType T>
inline T TExp(T value) 
{
    return static_cast<T>(std::exp(value));
}

template<>
inline float TExp(float value)
{
    return std::expf(value);
}

template<>
inline long double TExp(long double value)
{
    return std::expl(value);
}

template<FloatingType T>
inline T TEpsilon()
{
    return static_cast<T>(SLUG_F128_EPSILON);
}

template<>
inline float TEpsilon()
{
    return SLUG_F32_EPSILON;
}

template<>
inline double TEpsilon()
{
    return SLUG_F64_EPSILON;
}

template<>
inline long double TEpsilon()
{
    return SLUG_F128_EPSILON;
}

template<FloatingType T>
inline bool TEqual(T v0, T v1)
{
    return math::TAbs(v0 - v1) < math::TEpsilon<T>();
}


}
