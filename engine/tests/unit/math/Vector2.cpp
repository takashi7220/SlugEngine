#include <gtest/gtest.h>
#include <cmath>
#include "math/Math.hpp"
#include "math/Vector2.hpp"

using namespace slug::math;

// テストの許容誤差（Math.hpp の TEpsilon に依存）
template <class T>
static constexpr T kTol()
{
    return static_cast<T>(10) * TEpsilon<T>();
}

TEST(Vector2_Basic, DefaultConstruct_And_FieldAssign)
{
    Vector2 v;               // デフォルト構築
    v.x = static_cast<ValueType>(0);
    v.y = static_cast<ValueType>(0);

    EXPECT_EQ(v.x, static_cast<ValueType>(0));
    EXPECT_EQ(v.y, static_cast<ValueType>(0));

    // 値の変更が素直に反映される
    v.x = static_cast<ValueType>(3);
    v.y = static_cast<ValueType>(4);
    EXPECT_EQ(v.x, static_cast<ValueType>(3));
    EXPECT_EQ(v.y, static_cast<ValueType>(4));
}

TEST(Vector2_Normalize, ReturnsUnitVector_And_DoesNotMutateOriginal)
{
    Vector2 v;
    v.x = static_cast<ValueType>(3);
    v.y = static_cast<ValueType>(4);

    // Normalize() は新しいベクトルを返す（元は不変の前提）
    Vector2 n = v.Normalize();

    // 元は変わらない
    EXPECT_EQ(v.x, static_cast<ValueType>(3));
    EXPECT_EQ(v.y, static_cast<ValueType>(4));

    // 期待される単位ベクトル (0.6, 0.8)
    EXPECT_NEAR(n.x, 0.6, kTol<float>());
    EXPECT_NEAR(n.y, 0.8, kTol<float>());

    // 長さ ≈ 1
    float len = n.Length();
    EXPECT_NEAR(len, 1.0, kTol<float>());
}

TEST(Vector2_Normalize, SelfNormalizeMutatesAndProducesUnit)
{
    Vector2 v;
    v.x = static_cast<ValueType>(-5);
    v.y = static_cast<ValueType>(0);

    v.SelfNormalize();

    // 期待される単位ベクトル (-1, 0)
    EXPECT_NEAR(static_cast<double>(v.x), -1.0, kTol<double>());
    EXPECT_NEAR(static_cast<double>(v.y), 0.0, kTol<double>());

    double len = std::sqrt(static_cast<double>(v.x) * static_cast<double>(v.x) +
                           static_cast<double>(v.y) * static_cast<double>(v.y));
    EXPECT_NEAR(len, 1.0, kTol<double>());
}

// ゼロベクトルの正規化の挙動：多くの実装では (0,0) を返す。
// 実装が別挙動（例：例外・NaN など）の場合はここを調整してください。
TEST(Vector2_Normalize, ZeroVectorNormalizesToZeroVector)
{
    Vector2 v {};
    v.x = static_cast<ValueType>(0);
    v.y = static_cast<ValueType>(0);

    Vector2 n = v.Normalize();
    EXPECT_NEAR(static_cast<double>(n.x), 0.0, kTol<double>());
    EXPECT_NEAR(static_cast<double>(n.y), 0.0, kTol<double>());

    // SelfNormalize でも (0,0) を維持する前提
    v.SelfNormalize();
    EXPECT_NEAR(static_cast<double>(v.x), 0.0, kTol<double>());
    EXPECT_NEAR(static_cast<double>(v.y), 0.0, kTol<double>());
}

TEST(Vector2_Cross, CrossProduct2D_ZScalarValue)
{
    // 2D の外積（z 成分）: x1*y2 - x2*y1
    Vector2 a, b;
    a.x = static_cast<ValueType>(2);
    a.y = static_cast<ValueType>(1);
    b.x = static_cast<ValueType>(3);
    b.y = static_cast<ValueType>(5);

    // 2*5 - 3*1 = 7
    ValueType c = a.Cross(b);
    EXPECT_NEAR(static_cast<double>(c), 7.0, kTol<double>());

    // 平行ベクトルなら 0 に近い
    b.x = static_cast<ValueType>(4);
    b.y = static_cast<ValueType>(2); // a の 2 倍
    EXPECT_NEAR(static_cast<double>(a.Cross(b)), 0.0, kTol<double>());
}

TEST(Vector2_Normalize, Idempotency_Check)
{
    // 正規化済みベクトルに対する Normalize は変わらない前提
    Vector2 v;
    v.x = static_cast<ValueType>(0.6);
    v.y = static_cast<ValueType>(0.8);

    Vector2 n1 = v.Normalize();
    Vector2 n2 = n1.Normalize();

    EXPECT_NEAR(static_cast<double>(n1.x), static_cast<double>(n2.x), kTol<double>());
    EXPECT_NEAR(static_cast<double>(n1.y), static_cast<double>(n2.y), kTol<double>());
}
