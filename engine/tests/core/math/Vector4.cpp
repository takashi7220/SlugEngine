#include <gtest/gtest.h>
#include <cmath>
#include "math/Vector4.hpp"

using namespace slug::math;

// ---- 基本（コンストラクタ／等価演算） ---------------------------------
TEST(Vector4Test, ConstructAndEquality)
{
    Vector4 a(1.0f, 2.0f, 3.0f, 1.0f);
    Vector4 b(a);
    EXPECT_EQ(a, b);

    Vector4 c(1.0f, 2.0f, 3.0f, 0.0f);
    EXPECT_NE(a, c); // w が違えば不等
}

// ---- 加減算：x,y,z のみ加減、w は不変 -------------------------------
TEST(Vector4Test, AddSubtractKeepsW)
{
    Vector4 v1(1, 2, 3, 1);
    Vector4 v2(2, 3, 4, 1);

    Vector4 sum = v1 + v2;
    Vector4 diff = v1 - v2;

    EXPECT_EQ(sum, Vector4(3, 5, 7, 1));    // w は 1 のまま
    EXPECT_EQ(diff, Vector4(-1, -1, -1, 1)); // w は 1 のまま
}

// ---- スカラー乗除：x,y,z のみスケール、w は不変 ----------------------
TEST(Vector4Test, ScalarMulDivKeepsW)
{
    Vector4 v(1, 2, 3, 1);

    Vector4 m = v * 2.0f;
    EXPECT_EQ(m, Vector4(2, 4, 6, 1)); // w は 1 のまま

    Vector4 d = v / 2.0f;
    EXPECT_EQ(d, Vector4(0.5f, 1.0f, 1.5f, 1)); // w は 1 のまま
}

// ---- Dot：x,y,z のみで内積 ------------------------------------------
TEST(Vector4Test, DotUsesOnlyXYZ)
{
    Vector4 v1(1, 2, 3, 1);
    Vector4 v2(2, 3, 4, 1);
    // 1*2 + 2*3 + 3*4 = 2 + 6 + 12 = 20 （w は加算しない）
    EXPECT_FLOAT_EQ(v1.Dot(v2), 20.0f);
}

// ---- Cross：x,y,z のみで外積し Vector3 を返す ------------------------
TEST(Vector4Test, CrossReturnsVector3OfXYZ)
{
    Vector4 v1(1, 2, 3, 1);
    Vector4 v2(2, 3, 4, 1);

    Vector3 c = v1.Cross(v2);
    // (1,2,3) × (2,3,4) = (-1, 2, -1)
    EXPECT_EQ(c, Vector3(-1, 2, -1));

    // 直交性: v1.xyz・c == 0, v2.xyz・c == 0
    float dot1 = v1.x * c.x + v1.y * c.y + v1.z * c.z;
    float dot2 = v2.x * c.x + v2.y * c.y + v2.z * c.z;
    EXPECT_NEAR(dot1, 0.0f, 1e-6f);
    EXPECT_NEAR(dot2, 0.0f, 1e-6f);
}

// ---- Length：x,y,z のみで長さ ---------------------------------------
TEST(Vector4Test, LengthUsesOnlyXYZ)
{
    Vector4 v(3, 4, 5, 1);
    // sqrt(3^2 + 4^2 + 5^2) = sqrt(50) ≈ 7.0710678…
    EXPECT_NEAR(v.Length(), 7.0710678f, 1e-5f);
}

// ---- Normalize：x,y,z を正規化し w は不変 ----------------------------
TEST(Vector4Test, NormalizeKeepsWAndMakesXYZUnit)
{
    Vector4 v(3, 4, 5, 1);
    Vector4 n = v.Normalize();

    float len = std::sqrt(n.x * n.x + n.y * n.y + n.z * n.z);
    EXPECT_NEAR(len, 1.0f, 1e-6f);
    EXPECT_NEAR(n.x, 3.0f / std::sqrt(50.0f), 1e-3f);
    EXPECT_NEAR(n.y, 4.0f / std::sqrt(50.0f), 1e-3f);
    EXPECT_NEAR(n.z, 5.0f / std::sqrt(50.0f), 1e-3f);
    EXPECT_NEAR(n.w, 1.0f, 1e-6f); // w は変わらない
}

// ---- SelfNormalize がある場合（存在しないならこのテストは外す） ------
TEST(Vector4Test, SelfNormalizeKeepsWAndMakesXYZUnit_InPlace)
{
    Vector4 v(0, 0, 0, 1);
    // ゼロベクトルはそのまま（分母 0 回避の仕様想定）
    v.SelfNormalize();
    EXPECT_EQ(v, Vector4(0, 0, 0, 1));

    v = Vector4(-5, 0, 0, 1);
    v.SelfNormalize();
    EXPECT_NEAR(v.x, -1.0f, 1e-6f);
    EXPECT_NEAR(v.y, 0.0f, 1e-6f);
    EXPECT_NEAR(v.z, 0.0f, 1e-6f);
    EXPECT_NEAR(v.w, 1.0f, 1e-6f);
}

// ---- 同次（w=0 と w=1）の取り扱いを確認 ------------------------------
TEST(Vector4Test, HomogeneousPointAndDirection)
{
    Vector4 p(1, 2, 3, 1); // point（w=1）
    Vector4 d(1, 0, 0, 0); // direction（w=0）

    // point + direction -> point（w=1 を維持）
    Vector4 moved = p + d;
    EXPECT_EQ(moved, Vector4(2, 2, 3, 1));

    // direction のスケーリングでも w=0 を維持
    Vector4 d2 = d * 2.0f;
    EXPECT_EQ(d2, Vector4(2, 0, 0, 1.0f));
}
