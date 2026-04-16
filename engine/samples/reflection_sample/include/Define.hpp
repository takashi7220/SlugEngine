#include "core/reflection/Attribute.hpp"
#include "math/Vector2.hpp"
#include "math/Vector3.hpp"
#include "math/Vector4.hpp"
#include "math/Matrix3x3.hpp"
#include "math/Matrix4x4.hpp"
#include "math/Quaternion.hpp"

namespace slug::reflection_sample
{

/// @SLUG_GENERATE_REFLECTION
enum class TestType
{
    A = 0,
    B = 1 << 0,
    C = 1 << 2,
    D = 1 << 3
};

/// @SLUG_GENERATE_REFLECTION
class BaseTest
{
public:
    /// @SLUG_GENERATE_REFLECTION
    enum class TestType
    {
        A = 0,
        B = 1,
        C = 100,
        D = -1
    };

public:
    SLUG_GENERATE_REFLECTION(BaseTest)

private:
    bool flagt = true;
    TestType type = TestType::A;
    core::TVector<float> tmps2 = {};
    core::TVector<math::Matrix3x3> tmps3 = {};
    math::Matrix3x3 matrix33 = {};
    math::Matrix4x4 matrix44 = {};
    math::Quaternion quaternion = {};

};

#if 1

class Test
{
public:

    class TestTest : public BaseTest
    {
    public:
        SLUG_GENERATE_REFLECTION(TestTest)

    private:
        /// @Ignore(true) @Default(true)
        bool flag = true;

        /// @DisplayName(座標) @Description(座標です。) @EditCondition(flag) @Max(1.0) @Min(-1.0) @Step(0.1) @Brawsable(false) @Default(0.0f,0.0f,0.0f)
        slug::math::Vector2 position = { 0.0f, 0.0f};

        /// @DisplayName(回転) @Description(回転です) @EditCondition(flag) @Max(1.0) @Min(-1.0) @Step(0.1) @Brawsable(false) @Default(0.0f,0.0f,0.0f)
        slug::math::Vector3 rotation = { 0.0f, 0.0f, 0.0f };

        /// @DisplayName(スケール) @Description(スケールです。) @EditCondition(flag) @Max(1.0) @Min(-1.0) @Step(0.1) @Brawsable(false) @Default(0.0f,0.0f,0.0f)
        slug::math::Vector4 scale = { 1.0f, 1.0f, 1.0f , 1.0f};
    };
public:
    SLUG_GENERATE_REFLECTION(Test)

private:
    /// @Ignore(true) @Default(true)
    bool flag = true;

    /// @DisplayName(座標) @Description(座標です。) @EditCondition(flag) @Max(1.0) @Min(-1.0) @Step(0.1) @Brawsable(false) @Default(0.0f,0.0f,0.0f)
    slug::math::Vector3 position = { 0.0f, 0.0f, 0.0f };

    /// @DisplayName(回転) @Description(回転です) @EditCondition(flag) @Max(1.0) @Min(-1.0) @Step(0.1) @Brawsable(false) @Default(0.0f,0.0f,0.0f)
    slug::math::Vector3 rotation = { 0.0f, 0.0f, 0.0f };

    /// @DisplayName(スケール) @Description(スケールです。) @EditCondition(flag) @Max(1.0) @Min(-1.0) @Step(0.1) @Brawsable(false) @Default(0.0f,0.0f,0.0f)
    slug::math::Vector3 scale = { 1.0f, 1.0f, 1.0f };
};
#endif
}

class Test : public reflection_sample::Test
{
public:
    struct TestSturct
    {
    public:
        SLUG_GENERATE_REFLECTION(Test)

    private:
        /// @Ignore(true) @Default(true)
        bool flag = true;

        /// @DisplayName(座標) @Description(座標です。) @EditCondition(flag) @Max(1.0) @Min(-1.0) @Step(0.1) @Brawsable(false) @Default(0.0f,0.0f,0.0f)
        slug::math::Vector3 position = { 0.0f, 0.0f, 0.0f };

        /// @DisplayName(回転) @Description(回転です) @EditCondition(flag) @Max(1.0) @Min(-1.0) @Step(0.1) @Brawsable(false) @Default(0.0f,0.0f,0.0f)
        slug::math::Vector3 rotation = { 0.0f, 0.0f, 0.0f };

        /// @DisplayName(スケール) @Description(スケールです。) @EditCondition(flag) @Max(1.0) @Min(-1.0) @Step(0.1) @Brawsable(false) @Default(0.0f,0.0f,0.0f)
        slug::math::Vector3 scale = { 1.0f, 1.0f, 1.0f };
    };
public:
    SLUG_GENERATE_REFLECTION(Test)

private:
    /// @Ignore(true) @Default(true)
    bool flag = true;

    /// @DisplayName(座標) @Description(座標です。) @EditCondition(flag) @Max(1.0) @Min(-1.0) @Step(0.1) @Brawsable(false) @Default(0.0f,0.0f,0.0f)
    slug::math::Vector3 position = { 0.0f, 0.0f, 0.0f };

    /// @DisplayName(回転) @Description(回転です) @EditCondition(flag) @Max(1.0) @Min(-1.0) @Step(0.1) @Brawsable(false) @Default(0.0f,0.0f,0.0f)
    slug::math::Vector3 rotation = { 0.0f, 0.0f, 0.0f };

    /// @DisplayName(スケール) @Description(スケールです。) @EditCondition(flag) @Max(1.0) @Min(-1.0) @Step(0.1) @Brawsable(false) @Default(0.0f,0.0f,0.0f)
    slug::math::Vector3 scale = { 1.0f, 1.0f, 1.0f };
};
