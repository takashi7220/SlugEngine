#include <gtest/gtest.h>
#include <cmath>
#include "math/Math.hpp"
#include "math/Vector3.hpp"

using namespace slug::math;

template <class T>
static T tol()
{
    // Math.hpp の TEpsilon を基準に、少し余裕を持たせた誤差許容
    return static_cast<T>(10) * TEpsilon<T>();
}

TEST(Vector3_Construct, DefaultAndValueCtorAndCopy)
{
    Vector3 v0;  // default: (0,0,0)
    EXPECT_NEAR(static_cast<float>(v0.x), 0.0, tol<float>());
    EXPECT_NEAR(static_cast<float>(v0.y), 0.0, tol<float>());
    EXPECT_NEAR(static_cast<float>(v0.z), 0.0, tol<float>());

    Vector3 v1(static_cast<ValueType>(1),
               static_cast<ValueType>(2),
               static_cast<ValueType>(3));
    EXPECT_NEAR(static_cast<float>(v1.x), 1.0, tol<float>());
    EXPECT_NEAR(static_cast<float>(v1.y), 2.0, tol<float>());
    EXPECT_NEAR(static_cast<float>(v1.z), 3.0, tol<float>());

    Vector3 v2(v1); // copy
    EXPECT_NEAR(static_cast<float>(v2.x), 1.0, tol<float>());
    EXPECT_NEAR(static_cast<float>(v2.y), 2.0, tol<float>());
    EXPECT_NEAR(static_cast<float>(v2.z), 3.0, tol<float>());
}

TEST(Vector3_Static, ZeroAndOne)
{
    Vector3 z = Vector3::Zero();
    EXPECT_NEAR(static_cast<float>(z.x), 0.0, tol<float>());
    EXPECT_NEAR(static_cast<float>(z.y), 0.0, tol<float>());
    EXPECT_NEAR(static_cast<float>(z.z), 0.0, tol<float>());

    Vector3 o = Vector3::One();
    EXPECT_NEAR(static_cast<float>(o.x), 1.0, tol<float>());
    EXPECT_NEAR(static_cast<float>(o.y), 1.0, tol<float>());
    EXPECT_NEAR(static_cast<float>(o.z), 1.0, tol<float>());
}

TEST(Vector3_Equality, UsesTEqualComponentwise)
{
    Vector3 a(static_cast<ValueType>(1), static_cast<ValueType>(2), static_cast<ValueType>(3));
    Vector3 b = a;

    // ほんの僅かな差は TEpsilon による比較で等しいと見なされる
    Vector3 c(static_cast<ValueType>(1) + TEpsilon<ValueType>() * static_cast<ValueType>(0.5),
              static_cast<ValueType>(2) - TEpsilon<ValueType>() * static_cast<ValueType>(0.5),
              static_cast<ValueType>(3));
    EXPECT_TRUE(a == b);
    EXPECT_TRUE(a == c);

    // 明確に異なる
    Vector3 d(static_cast<ValueType>(1.01),
              static_cast<ValueType>(2.0),
              static_cast<ValueType>(3.0));
    EXPECT_FALSE(a == d);
}

TEST(Vector3_Arithmetic_VectorVector, AddSubMulDiv)
{
    Vector3 a(static_cast<ValueType>(1), static_cast<ValueType>(2), static_cast<ValueType>(4));
    Vector3 b(static_cast<ValueType>(3), static_cast<ValueType>(5), static_cast<ValueType>(8));

    Vector3 s = a + b;
    EXPECT_NEAR(static_cast<float>(s.x), 4.0, tol<float>());
    EXPECT_NEAR(static_cast<float>(s.y), 7.0, tol<float>());
    EXPECT_NEAR(static_cast<float>(s.z), 12.0, tol<float>());

    Vector3 d = b - a;
    EXPECT_NEAR(static_cast<float>(d.x), 2.0, tol<float>());
    EXPECT_NEAR(static_cast<float>(d.y), 3.0, tol<float>());
    EXPECT_NEAR(static_cast<float>(d.z), 4.0, tol<float>());

    Vector3 m = a * b;  // 要素積
    EXPECT_NEAR(static_cast<float>(m.x), 3.0, tol<float>());
    EXPECT_NEAR(static_cast<float>(m.y), 10.0, tol<float>());
    EXPECT_NEAR(static_cast<float>(m.z), 32.0, tol<float>());

    Vector3 q = b / a;  // 要素除算
    EXPECT_NEAR(static_cast<float>(q.x), 3.0, tol<float>());
    EXPECT_NEAR(static_cast<float>(q.y), 2.5, tol<float>());
    EXPECT_NEAR(static_cast<float>(q.z), 2.0, tol<float>());
}

TEST(Vector3_Arithmetic_Scalar, AddSubMulDiv)
{
    Vector3 a(static_cast<ValueType>(1), static_cast<ValueType>(2), static_cast<ValueType>(3));

    Vector3 s = a + static_cast<ValueType>(2);
    EXPECT_NEAR(static_cast<float>(s.x), 3.0, tol<float>());
    EXPECT_NEAR(static_cast<float>(s.y), 4.0, tol<float>());
    EXPECT_NEAR(static_cast<float>(s.z), 5.0, tol<float>());

    Vector3 d = a - static_cast<ValueType>(2);
    EXPECT_NEAR(static_cast<float>(d.x), -1.0, tol<float>());
    EXPECT_NEAR(static_cast<float>(d.y), 0.0, tol<float>());
    EXPECT_NEAR(static_cast<float>(d.z), 1.0, tol<float>());

    Vector3 m = a * static_cast<ValueType>(2);
    EXPECT_NEAR(static_cast<float>(m.x), 2.0, tol<float>());
    EXPECT_NEAR(static_cast<float>(m.y), 4.0, tol<float>());
    EXPECT_NEAR(static_cast<float>(m.z), 6.0, tol<float>());

    Vector3 q = a / static_cast<ValueType>(2);
    EXPECT_NEAR(static_cast<float>(q.x), 0.5, tol<float>());
    EXPECT_NEAR(static_cast<float>(q.y), 1.0, tol<float>());
    EXPECT_NEAR(static_cast<float>(q.z), 1.5, tol<float>());
}

TEST(Vector3_CompoundAssign, VectorAndScalar)
{
    Vector3 a(static_cast<ValueType>(1), static_cast<ValueType>(2), static_cast<ValueType>(3));
    Vector3 b(static_cast<ValueType>(10), static_cast<ValueType>(20), static_cast<ValueType>(30));

    Vector3 t = a;
    t += b;
    EXPECT_NEAR(static_cast<float>(t.x), 11.0, tol<float>());
    EXPECT_NEAR(static_cast<float>(t.y), 22.0, tol<float>());
    EXPECT_NEAR(static_cast<float>(t.z), 33.0, tol<float>());

    t = a; t -= b;
    EXPECT_NEAR(static_cast<float>(t.x), -9.0, tol<float>());
    EXPECT_NEAR(static_cast<float>(t.y), -18.0, tol<float>());
    EXPECT_NEAR(static_cast<float>(t.z), -27.0, tol<float>());

    t = a; t *= b;
    EXPECT_NEAR(static_cast<float>(t.x), 10.0, tol<float>());
    EXPECT_NEAR(static_cast<float>(t.y), 40.0, tol<float>());
    EXPECT_NEAR(static_cast<float>(t.z), 90.0, tol<float>());

    t = b; t /= a;
    EXPECT_NEAR(static_cast<float>(t.x), 10.0, tol<float>());
    EXPECT_NEAR(static_cast<float>(t.y), 10.0, tol<float>());
    EXPECT_NEAR(static_cast<float>(t.z), 10.0, tol<float>());

    // スカラー版
    t = a; t += static_cast<ValueType>(2);
    EXPECT_NEAR(static_cast<float>(t.x), 3.0, tol<float>());
    EXPECT_NEAR(static_cast<float>(t.y), 4.0, tol<float>());
    EXPECT_NEAR(static_cast<float>(t.z), 5.0, tol<float>());

    t = a; t -= static_cast<ValueType>(2);
    EXPECT_NEAR(static_cast<float>(t.x), -1.0, tol<float>());
    EXPECT_NEAR(static_cast<float>(t.y), 0.0, tol<float>());
    EXPECT_NEAR(static_cast<float>(t.z), 1.0, tol<float>());

    t = a; t *= static_cast<ValueType>(2);
    EXPECT_NEAR(static_cast<float>(t.x), 2.0, tol<float>());
    EXPECT_NEAR(static_cast<float>(t.y), 4.0, tol<float>());
    EXPECT_NEAR(static_cast<float>(t.z), 6.0, tol<float>());

    t = a; t /= static_cast<ValueType>(2);
    EXPECT_NEAR(static_cast<float>(t.x), 0.5, tol<float>());
    EXPECT_NEAR(static_cast<float>(t.y), 1.0, tol<float>());
    EXPECT_NEAR(static_cast<float>(t.z), 1.5, tol<float>());
}

TEST(Vector3_Metrics, DotLengthLengthSqr)
{
    Vector3 a(static_cast<ValueType>(1), static_cast<ValueType>(2), static_cast<ValueType>(3));
    Vector3 b(static_cast<ValueType>(4), static_cast<ValueType>(-5), static_cast<ValueType>(6));

    ValueType dot = a.Dot(b);  // 1*4 + 2*(-5) + 3*6 = 4 -10 +18 = 12
    EXPECT_NEAR(dot, 12.0, tol<float>());

    ValueType lenA2 = a.LengthSqr(); // 1 + 4 + 9 = 14
    EXPECT_NEAR(lenA2, 14.0, tol<float>());

    ValueType lenA = a.Length();     // sqrt(14)
    EXPECT_NEAR(lenA, std::sqrt(14.0), tol<float>());
}

TEST(Vector3_Normalize, ReturnsUnitVector_ZeroHandled)
{
    Vector3 a(static_cast<ValueType>(3), static_cast<ValueType>(4), static_cast<ValueType>(12));
    Vector3 n = a.Normalize();

    // 長さ ≈ 1
    double len = std::sqrt(static_cast<float>(n.x) * static_cast<float>(n.x) +
                           static_cast<float>(n.y) * static_cast<float>(n.y) +
                           static_cast<float>(n.z) * static_cast<float>(n.z));
    EXPECT_NEAR(len, 1.0, tol<float>());

    // 元のベクトルは不変
    EXPECT_NEAR(static_cast<float>(a.x), 3.0, tol<float>());
    EXPECT_NEAR(static_cast<float>(a.y), 4.0, tol<float>());
    EXPECT_NEAR(static_cast<float>(a.z), 12.0, tol<float>());

    // Zero ベクトルの正規化は Zero を返す仕様
    Vector3 z = Vector3::Zero();
    Vector3 nz = z.Normalize();
    EXPECT_NEAR(static_cast<float>(nz.x), 0.0, tol<float>());
    EXPECT_NEAR(static_cast<float>(nz.y), 0.0, tol<float>());
    EXPECT_NEAR(static_cast<float>(nz.z), 0.0, tol<float>());
}

TEST(Vector3_SelfNormalize, MutatesAndMakesUnit)
{
    Vector3 a(static_cast<ValueType>(-5), static_cast<ValueType>(0), static_cast<ValueType>(0));
    a.SelfNormalize();

    EXPECT_NEAR(static_cast<float>(a.x), -1.0, tol<float>());
    EXPECT_NEAR(static_cast<float>(a.y), 0.0, tol<float>());
    EXPECT_NEAR(static_cast<float>(a.z), 0.0, tol<float>());

    double len = std::sqrt(static_cast<float>(a.x) * static_cast<float>(a.x) +
                           static_cast<float>(a.y) * static_cast<float>(a.y) +
                           static_cast<float>(a.z) * static_cast<float>(a.z));
    EXPECT_NEAR(len, 1.0, tol<float>());

    // Zero ベクトルはそのまま
    Vector3 z = Vector3::Zero();
    z.SelfNormalize();
    EXPECT_NEAR(static_cast<float>(z.x), 0.0, tol<float>());
    EXPECT_NEAR(static_cast<float>(z.y), 0.0, tol<float>());
    EXPECT_NEAR(static_cast<float>(z.z), 0.0, tol<float>());
}

TEST(Vector3_Cross, OrthogonalityAndMagnitude)
{
    // 直交系の例
    Vector3 ex(static_cast<ValueType>(1), static_cast<ValueType>(0), static_cast<ValueType>(0));
    Vector3 ey(static_cast<ValueType>(0), static_cast<ValueType>(1), static_cast<ValueType>(0));
    Vector3 ez(static_cast<ValueType>(0), static_cast<ValueType>(0), static_cast<ValueType>(1));

    // e_x × e_y = e_z
    Vector3 c1 = ex.Cross(ey);
    EXPECT_TRUE(c1 == ez);

    // e_y × e_x = -e_z
    Vector3 c2 = ey.Cross(ex);
    EXPECT_NEAR(static_cast<float>(c2.x), 0.0, tol<float>());
    EXPECT_NEAR(static_cast<float>(c2.y), 0.0, tol<float>());
    EXPECT_NEAR(static_cast<float>(c2.z), -1.0, tol<float>());

    // 一般ベクトルでの性質 |a×b| = |a||b|sinθ, かつ a・(a×b)=0, b・(a×b)=0
    Vector3 a(static_cast<ValueType>(2), static_cast<ValueType>(3), static_cast<ValueType>(4));
    Vector3 b(static_cast<ValueType>(-5), static_cast<ValueType>(6), static_cast<ValueType>(7));
    Vector3 cx = a.Cross(b);

    // 直交性
    EXPECT_NEAR(static_cast<float>(a.Dot(cx)), 0.0, 1e-10);
    EXPECT_NEAR(static_cast<float>(b.Dot(cx)), 0.0, 1e-10);

    // 大きさの確認（数値的に厳しすぎない範囲で）
    double la = static_cast<float>(a.Length());
    double lb = static_cast<float>(b.Length());
    double dot = static_cast<float>(a.Dot(b));
    double sin_theta = std::sqrt(std::max(0.0, 1.0 - (dot * dot) / (la * la * lb * lb)));
    double lcx = std::sqrt(static_cast<float>(cx.LengthSqr()));
    EXPECT_NEAR(lcx, la * lb * sin_theta, 1e-6);
}

TEST(Vector3_Algebra, DistributiveAndScalarAssociativity)
{
    Vector3 a(static_cast<ValueType>(1), static_cast<ValueType>(2), static_cast<ValueType>(3));
    Vector3 b(static_cast<ValueType>(4), static_cast<ValueType>(5), static_cast<ValueType>(6));
    ValueType s = static_cast<ValueType>(3);

    // (a + b) * s == a*s + b*s   （要素積ではなくスカラー倍に関して）
    Vector3 left = (a + b) * s;
    Vector3 right = a * s + b * s;
    EXPECT_TRUE(left == right);

    // (a - b) / s == a/s - b/s
    left = (a - b) / s;
    right = a / s - b / s;
    EXPECT_TRUE(left == right);
}
