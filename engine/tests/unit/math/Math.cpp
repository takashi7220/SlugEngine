#include <gtest/gtest.h>
#include <cmath>
#include <limits>
#include "math/Math.hpp"

using namespace slug::math;

// ==== 基本ユーティリティ ====
template <class T>
static T ulp_epsilon()
{
    // Math.hpp の TEpsilon<T>() をそのまま利用
    return TEpsilon<T>();
}

template <class T>
static T near_tol()
{
    // 許容誤差を「実装の ε × 数倍」で設定（テスト不安定化を避けるため控えめに）
    return static_cast<T>(5) * ulp_epsilon<T>();
}

// ===============================
// 定数の妥当性
// ===============================
TEST(MathConstants, MinValuesMatchCfloat)
{
    EXPECT_FLOAT_EQ(SLUG_F32_MIN, FLT_MIN);
    EXPECT_DOUBLE_EQ(SLUG_F64_MIN, DBL_MIN);
    EXPECT_EQ(SLUG_F128_MIN, LDBL_MIN);
}

TEST(MathConstants, EpsilonValuesMatchCfloat)
{
    EXPECT_FLOAT_EQ(SLUG_F32_EPSILON, FLT_EPSILON);
    EXPECT_DOUBLE_EQ(SLUG_F64_EPSILON, DBL_EPSILON);
    EXPECT_EQ(SLUG_F128_EPSILON, LDBL_EPSILON);
}

TEST(MathConstants, PiValuesReasonable)
{
    // 既知の円周率に十分近いこと
    EXPECT_NEAR(SLUG_F32_PI, 3.14159265f, 1e-7f);
    EXPECT_NEAR(SLUG_F64_PI, 3.141592653589793, 1e-15);
}

// ===============================
// TMax / TMin / TAbs
// ===============================
TEST(MathFuncs, TMaxMinWorksForInts)
{
    EXPECT_EQ(TMax(2, 5), 5);
    EXPECT_EQ(TMin(2, 5), 2);
    EXPECT_EQ(TMax(-3, -7), -3);
    EXPECT_EQ(TMin(-3, -7), -7);
}

TEST(MathFuncs, TMaxMinWorksForFloats)
{
    EXPECT_FLOAT_EQ(TMax(2.5f, 2.0f), 2.5f);
    EXPECT_FLOAT_EQ(TMin(2.5f, 2.0f), 2.0f);
}

TEST(MathFuncs, TAbsWorks)
{
    EXPECT_EQ(TAbs(-3), 3);
    EXPECT_EQ(TAbs(3), 3);
    EXPECT_FLOAT_EQ(TAbs(-2.5f), 2.5f);
}

// ===============================
// TSqrt
// ===============================
TEST(MathFuncs, TSqrtBasic)
{
    EXPECT_DOUBLE_EQ(TSqrt(0.0), 0.0);
    EXPECT_DOUBLE_EQ(TSqrt(1.0), 1.0);
    EXPECT_NEAR(TSqrt(4.0), 2.0, near_tol<double>());
    EXPECT_NEAR(TSqrt(2.0), std::sqrt(2.0), near_tol<double>());
}

// ===============================
// TClamp / TSaturate
// ===============================
TEST(MathFuncs, TClampClampsToRange)
{
    EXPECT_EQ(TClamp(5, 0, 10), 5);
    EXPECT_EQ(TClamp(-3, 0, 10), 0);
    EXPECT_EQ(TClamp(12, 0, 10), 10);

    EXPECT_FLOAT_EQ(TClamp(0.25f, 0.0f, 1.0f), 0.25f);
    EXPECT_FLOAT_EQ(TClamp(-0.1f, 0.0f, 1.0f), 0.0f);
    EXPECT_FLOAT_EQ(TClamp(1.2f, 0.0f, 1.0f), 1.0f);
}

TEST(MathFuncs, TSaturateClampsTo01)
{
    EXPECT_FLOAT_EQ(TSaturate(-0.5f), 0.0f);
    EXPECT_FLOAT_EQ(TSaturate(0.2f), 0.2f);
    EXPECT_FLOAT_EQ(TSaturate(1.5f), 1.0f);

    EXPECT_NEAR(TSaturate(-0.1), 0.0, near_tol<double>());
    EXPECT_NEAR(TSaturate(1.1), 1.0, near_tol<double>());
}

// ===============================
// TLerp
// ===============================
TEST(MathFuncs, TLerpInterpolates)
{
    // 端点一致
    EXPECT_DOUBLE_EQ(TLerp(0.0, 10.0, 0.0), 0.0);
    EXPECT_DOUBLE_EQ(TLerp(0.0, 10.0, 1.0), 10.0);
    // 中間
    EXPECT_NEAR(TLerp(0.0, 10.0, 0.25), 2.5, near_tol<double>());
    EXPECT_NEAR(TLerp(-2.0, 2.0, 0.5), 0.0, near_tol<double>());

    // 整数 a,b に対して実数 r でも動作
    EXPECT_NEAR(TLerp(0, 100, 0.3), 30.0, near_tol<double>());
}

// ===============================
// 三角関数 Sin/Cos/Tan
// ===============================
TEST(MathFuncs, TrigFunctionsBasic)
{
    // sin(0)=0, cos(0)=1, tan(0)=0
    EXPECT_NEAR(Sin(0.0), 0.0, near_tol<double>());
    EXPECT_NEAR(Cos(0.0), 1.0, near_tol<double>());
    EXPECT_NEAR(Tan(0.0), 0.0, near_tol<double>());

    // sin(pi/2)=1, cos(pi/2)=0
    const double half_pi = SLUG_F64_PI / 2.0;
    EXPECT_NEAR(Sin(half_pi), 1.0, 1e-12);
    EXPECT_NEAR(Cos(half_pi), 0.0, 1e-12);

    // sin(pi)=0, cos(pi)=-1, tan(pi)=0
    EXPECT_NEAR(Sin(SLUG_F64_PI), 0.0, 1e-12);
    EXPECT_NEAR(Cos(SLUG_F64_PI), -1.0, 1e-12);
    EXPECT_NEAR(Tan(SLUG_F64_PI), 0.0, 1e-12);
}

// ===============================
// TPow / TExp
// ===============================
TEST(MathFuncs, TPowWorksForCommonCases)
{
    EXPECT_DOUBLE_EQ(TPow(2.0, 3.0), 8.0);
    EXPECT_NEAR(TPow(9.0, 0.5), 3.0, near_tol<double>());
    EXPECT_NEAR(TPow(5.0, -1.0), 1.0 / 5.0, near_tol<double>());

    EXPECT_FLOAT_EQ(TPow(2.0f, 3.0f), 8.0f);
}

TEST(MathFuncs, TExpMatchesStd)
{
    EXPECT_NEAR(TExp(0.0), 1.0, near_tol<double>());
    EXPECT_NEAR(TExp(1.0), std::exp(1.0), near_tol<double>());
    EXPECT_NEAR(TExp(-2.0), std::exp(-2.0), near_tol<double>());

    EXPECT_NEAR(TExp(0.0f), 1.0f, near_tol<float>());
}

// ===============================
// TEpsilon / TEqual
// ===============================
TEST(MathFuncs, TEpsilonMatchesCfloat)
{
    EXPECT_FLOAT_EQ(TEpsilon<float>(), FLT_EPSILON);
    EXPECT_DOUBLE_EQ(TEpsilon<double>(), DBL_EPSILON);
    EXPECT_EQ(TEpsilon<long double>(), LDBL_EPSILON);
}

TEST(MathFuncs, TEqualUsesEpsilon)
{
    // 代表例：0.1 + 0.2 ≈ 0.3
    const double a = 0.1 + 0.2;
    const double b = 0.3;
    EXPECT_TRUE(TEqual(a, b));

    // 明確に異なる値は false
    EXPECT_FALSE(TEqual(1.0, 1.0 + 1e-2));
}
