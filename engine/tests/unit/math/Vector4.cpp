// Vector4Test.cpp

#include <gtest/gtest.h>
#include <cmath>

#include "math/Vector3.hpp"
#include "math/Vector4.hpp"


namespace
{
constexpr float kEpsilon = 1.0e-5f;

void ExpectVector4Near(
    const slug::math::Vector4& actual,
    float x,
    float y,
    float z,
    float w,
    float epsilon = kEpsilon)
{
    EXPECT_NEAR(actual.x, x, epsilon);
    EXPECT_NEAR(actual.y, y, epsilon);
    EXPECT_NEAR(actual.z, z, epsilon);
    EXPECT_NEAR(actual.w, w, epsilon);
}

void ExpectVector3Near(
    const slug::math::Vector3& actual,
    float x,
    float y,
    float z,
    float epsilon = kEpsilon)
{
    EXPECT_NEAR(actual.x, x, epsilon);
    EXPECT_NEAR(actual.y, y, epsilon);
    EXPECT_NEAR(actual.z, z, epsilon);
}
}

// ============================================================
// Constructor / Static
// ============================================================

TEST(Vector4Test, DefaultConstructor)
{
    slug::math::Vector4 v;

    ExpectVector4Near(v, 0.0f, 0.0f, 0.0f, 0.0f);
}

TEST(Vector4Test, ConstructorWithValues)
{
    slug::math::Vector4 v(1.0f, 2.0f, 3.0f, 4.0f);

    ExpectVector4Near(v, 1.0f, 2.0f, 3.0f, 4.0f);
}

TEST(Vector4Test, CopyConstructor)
{
    slug::math::Vector4 src(1.0f, 2.0f, 3.0f, 4.0f);
    slug::math::Vector4 dst(src);

    ExpectVector4Near(dst, 1.0f, 2.0f, 3.0f, 4.0f);
}

TEST(Vector4Test, ConstructorFromVector3SetsWToOne)
{
    slug::math::Vector3 src;
    src.x = 1.0f;
    src.y = 2.0f;
    src.z = 3.0f;

    slug::math::Vector4 v(src);

    ExpectVector4Near(v, 1.0f, 2.0f, 3.0f, 1.0f);
}

TEST(Vector4Test, ConstructorFromVector3WithW)
{
    slug::math::Vector3 src;
    src.x = 1.0f;
    src.y = 2.0f;
    src.z = 3.0f;

    slug::math::Vector4 v(src, 9.0f);

    ExpectVector4Near(v, 1.0f, 2.0f, 3.0f, 9.0f);
}

TEST(Vector4Test, Zero)
{
    slug::math::Vector4 v = slug::math::Vector4::Zero();

    // 提示コードでは Zero() は (0, 0, 0, 1)
    ExpectVector4Near(v, 0.0f, 0.0f, 0.0f, 1.0f);
}

TEST(Vector4Test, One)
{
    slug::math::Vector4 v = slug::math::Vector4::One();

    ExpectVector4Near(v, 1.0f, 1.0f, 1.0f, 1.0f);
}

TEST(Vector4Test, Alignment)
{
    EXPECT_EQ(alignof(slug::math::Vector4), 16);
    EXPECT_EQ(sizeof(slug::math::Vector4) % 16, 0);
}

// ============================================================
// operator==
// ============================================================

TEST(Vector4Test, EqualOperatorReturnsTrueForSameValues)
{
    slug::math::Vector4 a(1.0f, 2.0f, 3.0f, 4.0f);
    slug::math::Vector4 b(1.0f, 2.0f, 3.0f, 4.0f);

    EXPECT_TRUE(a == b);
}

TEST(Vector4Test, EqualOperatorReturnsFalseForDifferentX)
{
    slug::math::Vector4 a(1.0f, 2.0f, 3.0f, 4.0f);
    slug::math::Vector4 b(9.0f, 2.0f, 3.0f, 4.0f);

    EXPECT_FALSE(a == b);
}

TEST(Vector4Test, EqualOperatorReturnsFalseForDifferentY)
{
    slug::math::Vector4 a(1.0f, 2.0f, 3.0f, 4.0f);
    slug::math::Vector4 b(1.0f, 9.0f, 3.0f, 4.0f);

    EXPECT_FALSE(a == b);
}

TEST(Vector4Test, EqualOperatorReturnsFalseForDifferentZ)
{
    slug::math::Vector4 a(1.0f, 2.0f, 3.0f, 4.0f);
    slug::math::Vector4 b(1.0f, 2.0f, 9.0f, 4.0f);

    EXPECT_FALSE(a == b);
}

TEST(Vector4Test, EqualOperatorReturnsFalseForDifferentW)
{
    slug::math::Vector4 a(1.0f, 2.0f, 3.0f, 4.0f);
    slug::math::Vector4 b(1.0f, 2.0f, 3.0f, 9.0f);

    EXPECT_FALSE(a == b);
}

// ============================================================
// Vector4 arithmetic
// ============================================================

TEST(Vector4Test, AddVector4)
{
    slug::math::Vector4 a(1.0f, 2.0f, 3.0f, 4.0f);
    slug::math::Vector4 b(10.0f, 20.0f, 30.0f, 40.0f);

    slug::math::Vector4 result = a + b;

    ExpectVector4Near(result, 11.0f, 22.0f, 33.0f, 44.0f);
}

TEST(Vector4Test, SubtractVector4)
{
    slug::math::Vector4 a(10.0f, 20.0f, 30.0f, 40.0f);
    slug::math::Vector4 b(1.0f, 2.0f, 3.0f, 4.0f);

    slug::math::Vector4 result = a - b;

    ExpectVector4Near(result, 9.0f, 18.0f, 27.0f, 36.0f);
}

TEST(Vector4Test, MultiplyVector4)
{
    slug::math::Vector4 a(1.0f, 2.0f, 3.0f, 4.0f);
    slug::math::Vector4 b(10.0f, 20.0f, 30.0f, 40.0f);

    slug::math::Vector4 result = a * b;

    ExpectVector4Near(result, 10.0f, 40.0f, 90.0f, 160.0f);
}

TEST(Vector4Test, DivideVector4)
{
    slug::math::Vector4 a(10.0f, 20.0f, 30.0f, 40.0f);
    slug::math::Vector4 b(2.0f, 4.0f, 5.0f, 8.0f);

    slug::math::Vector4 result = a / b;

    ExpectVector4Near(result, 5.0f, 5.0f, 6.0f, 5.0f);
}

// ============================================================
// Scalar arithmetic
// ============================================================

TEST(Vector4Test, AddScalar)
{
    slug::math::Vector4 a(1.0f, 2.0f, 3.0f, 4.0f);

    slug::math::Vector4 result = a + 10.0f;

    ExpectVector4Near(result, 11.0f, 12.0f, 13.0f, 14.0f);
}

TEST(Vector4Test, SubtractScalar)
{
    slug::math::Vector4 a(10.0f, 20.0f, 30.0f, 40.0f);

    slug::math::Vector4 result = a - 5.0f;

    ExpectVector4Near(result, 5.0f, 15.0f, 25.0f, 35.0f);
}

TEST(Vector4Test, MultiplyScalar)
{
    slug::math::Vector4 a(1.0f, 2.0f, 3.0f, 4.0f);

    slug::math::Vector4 result = a * 2.0f;

    ExpectVector4Near(result, 2.0f, 4.0f, 6.0f, 8.0f);
}

TEST(Vector4Test, DivideScalar)
{
    slug::math::Vector4 a(10.0f, 20.0f, 30.0f, 40.0f);

    slug::math::Vector4 result = a / 10.0f;

    ExpectVector4Near(result, 1.0f, 2.0f, 3.0f, 4.0f);
}

// ============================================================
// Vector4 compound assignment
// ============================================================

TEST(Vector4Test, AddAssignVector4)
{
    slug::math::Vector4 a(1.0f, 2.0f, 3.0f, 4.0f);
    slug::math::Vector4 b(10.0f, 20.0f, 30.0f, 40.0f);

    a += b;

    ExpectVector4Near(a, 11.0f, 22.0f, 33.0f, 44.0f);
}

TEST(Vector4Test, SubtractAssignVector4)
{
    slug::math::Vector4 a(10.0f, 20.0f, 30.0f, 40.0f);
    slug::math::Vector4 b(1.0f, 2.0f, 3.0f, 4.0f);

    a -= b;

    ExpectVector4Near(a, 9.0f, 18.0f, 27.0f, 36.0f);
}

TEST(Vector4Test, MultiplyAssignVector4)
{
    slug::math::Vector4 a(1.0f, 2.0f, 3.0f, 4.0f);
    slug::math::Vector4 b(10.0f, 20.0f, 30.0f, 40.0f);

    a *= b;

    ExpectVector4Near(a, 10.0f, 40.0f, 90.0f, 160.0f);
}

TEST(Vector4Test, DivideAssignVector4)
{
    slug::math::Vector4 a(10.0f, 20.0f, 30.0f, 40.0f);
    slug::math::Vector4 b(2.0f, 4.0f, 5.0f, 8.0f);

    a /= b;

    ExpectVector4Near(a, 5.0f, 5.0f, 6.0f, 5.0f);
}

// ============================================================
// Scalar compound assignment
// ============================================================

TEST(Vector4Test, AddAssignScalar)
{
    slug::math::Vector4 a(1.0f, 2.0f, 3.0f, 4.0f);

    a += 10.0f;

    ExpectVector4Near(a, 11.0f, 12.0f, 13.0f, 14.0f);
}

TEST(Vector4Test, SubtractAssignScalar)
{
    slug::math::Vector4 a(10.0f, 20.0f, 30.0f, 40.0f);

    a -= 5.0f;

    ExpectVector4Near(a, 5.0f, 15.0f, 25.0f, 35.0f);
}

TEST(Vector4Test, MultiplyAssignScalar)
{
    slug::math::Vector4 a(1.0f, 2.0f, 3.0f, 4.0f);

    a *= 2.0f;

    ExpectVector4Near(a, 2.0f, 4.0f, 6.0f, 8.0f);
}

TEST(Vector4Test, DivideAssignScalar)
{
    slug::math::Vector4 a(10.0f, 20.0f, 30.0f, 40.0f);

    a /= 10.0f;

    ExpectVector4Near(a, 1.0f, 2.0f, 3.0f, 4.0f);
}

// ============================================================
// Dot / DotXYZ
// ============================================================

TEST(Vector4Test, DotUsesXYZW)
{
    slug::math::Vector4 a(1.0f, 2.0f, 3.0f, 4.0f);
    slug::math::Vector4 b(10.0f, 20.0f, 30.0f, 40.0f);

    float result = a.Dot(b);

    // 1*10 + 2*20 + 3*30 + 4*40 = 300
    EXPECT_NEAR(result, 300.0f, kEpsilon);
}

TEST(Vector4Test, DotXYZIgnoresW)
{
    slug::math::Vector4 a(1.0f, 2.0f, 3.0f, 100.0f);
    slug::math::Vector4 b(10.0f, 20.0f, 30.0f, 200.0f);

    float result = a.DotXYZ(b);

    // 1*10 + 2*20 + 3*30 = 140
    EXPECT_NEAR(result, 140.0f, kEpsilon);
}

TEST(Vector4Test, DotWithZeroVector)
{
    slug::math::Vector4 a(1.0f, 2.0f, 3.0f, 4.0f);
    slug::math::Vector4 b(0.0f, 0.0f, 0.0f, 0.0f);

    float result = a.Dot(b);

    EXPECT_NEAR(result, 0.0f, kEpsilon);
}

// ============================================================
// Length / LengthXYZ
// ============================================================

TEST(Vector4Test, LengthUsesXYZW)
{
    slug::math::Vector4 v(1.0f, 2.0f, 2.0f, 4.0f);

    float result = v.Length();

    // sqrt(1 + 4 + 4 + 16) = 5
    EXPECT_NEAR(result, 5.0f, kEpsilon);
}

TEST(Vector4Test, LengthXYZIgnoresW)
{
    slug::math::Vector4 v(1.0f, 2.0f, 2.0f, 100.0f);

    float result = v.LengthXYZ();

    // sqrt(1 + 4 + 4) = 3
    EXPECT_NEAR(result, 3.0f, kEpsilon);
}

TEST(Vector4Test, LengthOfZeroVector)
{
    slug::math::Vector4 v(0.0f, 0.0f, 0.0f, 0.0f);

    EXPECT_NEAR(v.Length(), 0.0f, kEpsilon);
    EXPECT_NEAR(v.LengthXYZ(), 0.0f, kEpsilon);
}

// ============================================================
// Normalize / NormalizeXYZ
// ============================================================

TEST(Vector4Test, NormalizeUsesXYZW)
{
    slug::math::Vector4 v(1.0f, 2.0f, 2.0f, 4.0f);

    slug::math::Vector4 result = v.Normalize();

    // length = 5
    ExpectVector4Near(result, 0.2f, 0.4f, 0.4f, 0.8f);
}

TEST(Vector4Test, NormalizeXYZIgnoresWAndKeepsW)
{
    slug::math::Vector4 v(1.0f, 2.0f, 2.0f, 100.0f);

    slug::math::Vector4 result = v.NormalizeXYZ();

    // xyz length = 3
    // 提示コードでは w は元の値を維持する
    ExpectVector4Near(
        result,
        1.0f / 3.0f,
        2.0f / 3.0f,
        2.0f / 3.0f,
        100.0f);
}

TEST(Vector4Test, SelfNormalize)
{
    slug::math::Vector4 v(1.0f, 2.0f, 2.0f, 4.0f);

    v.SelfNormalize();

    ExpectVector4Near(v, 0.2f, 0.4f, 0.4f, 0.8f);
}

TEST(Vector4Test, SelfNormalizeXYZ)
{
    slug::math::Vector4 v(1.0f, 2.0f, 2.0f, 100.0f);

    v.SelfNormalizeXYZ();

    ExpectVector4Near(
        v,
        1.0f / 3.0f,
        2.0f / 3.0f,
        2.0f / 3.0f,
        100.0f);
}

TEST(Vector4Test, NormalizeResultLengthIsOne)
{
    slug::math::Vector4 v(1.0f, 2.0f, 2.0f, 4.0f);

    slug::math::Vector4 result = v.Normalize();

    EXPECT_NEAR(result.Length(), 1.0f, kEpsilon);
}

TEST(Vector4Test, NormalizeXYZResultLengthXYZIsOne)
{
    slug::math::Vector4 v(1.0f, 2.0f, 2.0f, 100.0f);

    slug::math::Vector4 result = v.NormalizeXYZ();

    EXPECT_NEAR(result.LengthXYZ(), 1.0f, kEpsilon);
    EXPECT_NEAR(result.w, 100.0f, kEpsilon);
}

// ============================================================
// Cross
// ============================================================

TEST(Vector4Test, CrossReturnsXYZCrossProduct)
{
    slug::math::Vector4 xAxis(1.0f, 0.0f, 0.0f, 123.0f);
    slug::math::Vector4 yAxis(0.0f, 1.0f, 0.0f, 456.0f);

    slug::math::Vector3 result = xAxis.Cross(yAxis);

    ExpectVector3Near(result, 0.0f, 0.0f, 1.0f);
}

TEST(Vector4Test, CrossIgnoresW)
{
    slug::math::Vector4 a(1.0f, 0.0f, 0.0f, 100.0f);
    slug::math::Vector4 b(0.0f, 1.0f, 0.0f, 200.0f);

    slug::math::Vector3 result = a.Cross(b);

    ExpectVector3Near(result, 0.0f, 0.0f, 1.0f);
}

TEST(Vector4Test, CrossReverseOrder)
{
    slug::math::Vector4 xAxis(1.0f, 0.0f, 0.0f, 0.0f);
    slug::math::Vector4 yAxis(0.0f, 1.0f, 0.0f, 0.0f);

    slug::math::Vector3 result = yAxis.Cross(xAxis);

    ExpectVector3Near(result, 0.0f, 0.0f, -1.0f);
}

TEST(Vector4Test, CrossParallelVectorsReturnsZero)
{
    slug::math::Vector4 a(1.0f, 2.0f, 3.0f, 0.0f);
    slug::math::Vector4 b(2.0f, 4.0f, 6.0f, 0.0f);

    slug::math::Vector3 result = a.Cross(b);

    ExpectVector3Near(result, 0.0f, 0.0f, 0.0f);
}
