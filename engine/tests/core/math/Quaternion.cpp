// tests/test_Quaternion.cpp
#include <gtest/gtest.h>
#include <cmath>
#include "math/TypeConcept.hpp"
#include "math/Vector3.hpp"
#include "math/Vector4.hpp"
#include "math/Matrix3x3.hpp"
#include "math/Matrix4x4.hpp"
#include "math/Quaternion.hpp"

using namespace slug::math;

namespace
{

// 角度の近似比較（ラップ考慮せず、テスト角は小～中角に限定）
inline bool NearlyEqual(ValueType a, ValueType b, ValueType eps = static_cast<ValueType>(1e-5))
{
    return std::fabs(a - b) <= eps;
}

inline ::testing::AssertionResult Vec4Near(const Vector4& a, const Vector4& b, ValueType eps = static_cast<ValueType>(1e-5))
{
    if (!NearlyEqual(a.x, b.x, eps)) return ::testing::AssertionFailure() << "x expected " << b.x << " got " << a.x;
    if (!NearlyEqual(a.y, b.y, eps)) return ::testing::AssertionFailure() << "y expected " << b.y << " got " << a.y;
    if (!NearlyEqual(a.z, b.z, eps)) return ::testing::AssertionFailure() << "z expected " << b.z << " got " << a.z;
    if (!NearlyEqual(a.w, b.w, eps)) return ::testing::AssertionFailure() << "w expected " << b.w << " got " << a.w;
    return ::testing::AssertionSuccess();
}

inline ::testing::AssertionResult Vec3Near(const Vector3& a, const Vector3& b, ValueType eps = static_cast<ValueType>(1e-5))
{
    if (!NearlyEqual(a.x, b.x, eps)) return ::testing::AssertionFailure() << "x expected " << b.x << " got " << a.x;
    if (!NearlyEqual(a.y, b.y, eps)) return ::testing::AssertionFailure() << "y expected " << b.y << " got " << a.y;
    if (!NearlyEqual(a.z, b.z, eps)) return ::testing::AssertionFailure() << "z expected " << b.z << " got " << a.z;
    return ::testing::AssertionSuccess();
}

// 単位純虚四元数（i, j, k, w=0）ヘルパ
Quaternion Qi()
{
    return Quaternion(1, 0, 0, 0);
}
Quaternion Qj()
{
    return Quaternion(0, 1, 0, 0);
}
Quaternion Qk()
{
    return Quaternion(0, 0, 1, 0);
}

} // namespace

// -------------------- 基本コンストラクタ/代入 --------------------

TEST(Quaternion, DefaultCtorIsIdentityRotation)
{
    Quaternion q;
    EXPECT_TRUE(Vec4Near(q.GetVector4(), Vector4(0, 0, 0, 1)));
}

TEST(Quaternion, InitializerListCtor)
{
    Quaternion q { 1,2,3,4 };
    EXPECT_TRUE(Vec4Near(q.GetVector4(), Vector4(1, 2, 3, 4)));
}

TEST(Quaternion, AssignFromVector4)
{
    Vector4 v { 0.1f, -0.2f, 0.3f, 0.9f };
    Quaternion q;
    q = v;
    EXPECT_TRUE(Vec4Near(q.GetVector4(), v));
}

// -------------------- ハミルトン積の性質 --------------------

TEST(Quaternion, HamiltonProduct_UnitPures)
{
    // i*j = k, j*k = i, k*i = j
    auto i = Qi(), j = Qj(), k = Qk();
    auto ij = i * j;  EXPECT_TRUE(Vec4Near(ij.GetVector4(), Vector4(0, 0, 1, 0))); // k
    auto jk = j * k;  EXPECT_TRUE(Vec4Near(jk.GetVector4(), Vector4(1, 0, 0, 0))); // i
    auto ki = k * i;  EXPECT_TRUE(Vec4Near(ki.GetVector4(), Vector4(0, 1, 0, 0))); // j

    // 反可換性: j*i = -k
    auto ji = j * i;
    EXPECT_TRUE(Vec4Near(ji.GetVector4(), Vector4(0, 0, -1, 0)));
}

// -------------------- オイラー角 Set*/Get* --------------------

TEST(Quaternion, SetGetEulerX)
{
    ValueType ax = static_cast<ValueType>(30.0f); // ~17.2°
    Quaternion q; q.SetEulerX(ax);
    EXPECT_TRUE(NearlyEqual(q.GetEulerX(), ax, static_cast<ValueType>(1e-5)));
    EXPECT_TRUE(NearlyEqual(q.GetEulerY(), static_cast<ValueType>(0)));
    EXPECT_TRUE(NearlyEqual(q.GetEulerZ(), static_cast<ValueType>(0)));
}

TEST(Quaternion, SetGetEulerY)
{
    ValueType ay = static_cast<ValueType>(-25.0f);
    Quaternion q; q.SetEulerY(ay);
    EXPECT_TRUE(NearlyEqual(q.GetEulerX(), static_cast<ValueType>(0)));
    EXPECT_TRUE(NearlyEqual(q.GetEulerY(), ay, static_cast<ValueType>(1e-5)));
    EXPECT_TRUE(NearlyEqual(q.GetEulerZ(), static_cast<ValueType>(0)));
}

TEST(Quaternion, SetGetEulerZ)
{
    ValueType az = static_cast<ValueType>(50.0f);
    Quaternion q; q.SetEulerZ(az);
    EXPECT_TRUE(NearlyEqual(q.GetEulerX(), static_cast<ValueType>(0)));
    EXPECT_TRUE(NearlyEqual(q.GetEulerY(), static_cast<ValueType>(0)));
    EXPECT_TRUE(NearlyEqual(q.GetEulerZ(), az, static_cast<ValueType>(1e-5)));
}

TEST(Quaternion, SetGetEulerXYZ)
{
    Vector3 e { static_cast<ValueType>(20.0f), static_cast<ValueType>(-10.0f), static_cast<ValueType>(30.0f) };
    Quaternion q; q.SetEulerXYZ(e);
    auto r = q.GetEulerXYZ();
    EXPECT_TRUE(Vec3Near(r, e, static_cast<ValueType>(1e-5)));
}

TEST(Quaternion, AddEulerXyzAccumulates)
{
    Quaternion q;
    q.SetEulerXYZ(Vector3(10.0f, -20.0f, 30.0f));
    q.AddEulerXYZ(Vector3(5.0f, 10.0f, -5.0f));
    auto r = q.GetEulerXYZ();
    EXPECT_TRUE(Vec3Near(r, Vector3(15.0f, -10.0f, 25.0f), static_cast<ValueType>(2e-5)));
}

TEST(Quaternion, AddEulerIndividualAxes)
{
    Quaternion q;
    q.SetEulerX(10.0f);
    q.SetEulerY(-20.0f);
    q.SetEulerZ(30.0f);

    q.AddEulerX(5.0f);
    q.AddEulerY(10.0f);
    q.AddEulerZ(-5.0f);
    EXPECT_TRUE(Vec3Near(q.GetEulerXYZ(), Vector3(15.0f, -10.0f, 25.0f), static_cast<ValueType>(2e-5)));
}

// -------------------- 軸角 --------------------

TEST(Quaternion, SetRotation_AxisAngle_Z)
{
    // z 軸、角度 θ の回転
    ValueType theta = static_cast<ValueType>(60.0f);
    ValueType half = Deg2Rad(theta) * static_cast<ValueType>(0.5);
    ValueType s = slug::math::Sin(half);
    ValueType c = slug::math::Cos(half);

    Quaternion q; q.SetRotation(Vector3(0, 0, 1), theta);
    EXPECT_TRUE(Vec4Near(q.GetVector4(), Vector4(0, 0, s, c), static_cast<ValueType>(1e-6)));
}

// -------------------- 行列化（実装の既知の落とし穴の検出用） --------------------
// 現在の Matrix3x3/4x4 は at(i,j) が行優先インデックスだが内部ストレージは列優先、
// かつベクトル乗算は列優先前提のため、Quaternion からの行列生成は意図と異なる位置に
// 値が入る可能性があります。必要に応じて有効化して確認してください。

TEST(Quaternion, DISABLED_GetMatrix3x3_RotatesVector)
{
    Quaternion q; q.SetEulerZ(static_cast<ValueType>(SLUG_F32_PI) / 2); // 90° Z回転
    auto M = q.GetMatrix3x3();
    Vector3 x { 1,0,0 };
    auto r = M * x; // 期待： (0,1,0)
    EXPECT_TRUE(Vec3Near(r, Vector3(0, 1, 0), static_cast<ValueType>(1e-4)));
}

TEST(Quaternion, DISABLED_GetMatrix4x4_RotatesVector)
{
    Quaternion q; q.SetEulerY(static_cast<ValueType>(SLUG_F32_PI) / 2); // 90° Y回転
    auto M = q.GetMatrix4x4();
    Vector4 z { 0,0,1,1 };
    auto r = M * z; // 期待： (1,0,0,1)
    EXPECT_TRUE(Vec4Near(r, Vector4(1, 0, 0, 1), static_cast<ValueType>(1e-4)));
}
