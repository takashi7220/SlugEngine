#include "core/color/Color.hpp"
#include <gtest/gtest.h>

using namespace slug::core;

namespace
{

void ExpectColorNear(const Color& color, float r, float g, float b, float a)
{
    EXPECT_FLOAT_EQ(color.r, r);
    EXPECT_FLOAT_EQ(color.g, g);
    EXPECT_FLOAT_EQ(color.b, b);
    EXPECT_FLOAT_EQ(color.a, a);
}

TEST(ColorTest, DefaultConstructorInitializesTransparentBlack)
{
    Color color;

    ExpectColorNear(color, 0.0f, 0.0f, 0.0f, 0.0f);
    EXPECT_EQ(color.colorSpace, ColorSpaceType::sRGB);
}

TEST(ColorTest, InitializerListUsesFirstFourValues)
{
    Color color = { 0.1f, 0.2f, 0.3f, 0.4f };

    ExpectColorNear(color, 0.1f, 0.2f, 0.3f, 0.4f);
}

TEST(ColorTest, ColorArithmeticWithMatchingColorSpace)
{
    Color left(0.8f, 0.6f, 0.4f, 0.2f);
    Color right(0.2f, 0.3f, 0.4f, 0.1f);

    ExpectColorNear(left + right, 1.0f, 0.9f, 0.8f, 0.3f);
    ExpectColorNear(left - right, 0.6f, 0.3f, 0.0f, 0.1f);
    ExpectColorNear(left * right, 0.16f, 0.18f, 0.16f, 0.02f);
    ExpectColorNear(left / right, 4.0f, 2.0f, 1.0f, 2.0f);
}

TEST(ColorTest, ScalarArithmetic)
{
    Color color(0.8f, 0.6f, 0.4f, 0.2f);

    ExpectColorNear(color + 0.2f, 1.0f, 0.8f, 0.6f, 0.4f);
    ExpectColorNear(color - 0.2f, 0.6f, 0.4f, 0.2f, 0.0f);
    ExpectColorNear(color * 2.0f, 1.6f, 1.2f, 0.8f, 0.4f);
    ExpectColorNear(color / 2.0f, 0.4f, 0.3f, 0.2f, 0.1f);
}

TEST(ColorTest, CompoundScalarArithmeticUpdatesInPlace)
{
    Color multiply(0.8f, 0.6f, 0.4f, 0.2f);
    multiply *= 2.0f;

    Color divide(0.8f, 0.6f, 0.4f, 0.2f);
    divide /= 2.0f;

    ExpectColorNear(multiply, 1.6f, 1.2f, 0.8f, 0.4f);
    ExpectColorNear(divide, 0.4f, 0.3f, 0.2f, 0.1f);
}

TEST(ColorSpaceUtilityTest, ConvertSRGBToRec709Linear)
{
    float r = 0.0f;
    float g = 0.0f;
    float b = 0.0f;

    const bool success = ColorSpaceUtility::Convert(
        r,
        g,
        b,
        ColorSpaceType::Rec709Linear,
        0.5f,
        0.25f,
        0.75f,
        ColorSpaceType::sRGB);

    EXPECT_TRUE(success);
    EXPECT_NEAR(r, 0.214041f, 0.000001f);
    EXPECT_NEAR(g, 0.050876f, 0.000001f);
    EXPECT_NEAR(b, 0.522522f, 0.000001f);
}

}
