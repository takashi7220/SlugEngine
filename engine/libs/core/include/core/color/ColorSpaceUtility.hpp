#pragma once

#include "math/Math.hpp"
#include "math/Vector3.hpp"
#include "math/Vector4.hpp"
#include "math/Matrix3x3.hpp"
#include "core/color/Spectrum.hpp"

namespace slug::core
{

enum class ColorSpaceType
{
    sRGB = 0,
    Rec709Linear,
    Rec2020Linear,
    XYZ
};

class ColorSpaceUtility
{
public:
    using ValueType = float;
    using ConvertFunc = math::Vector3(*)(ValueType, ValueType, ValueType);

    struct ColorSpaceConvert
    {
        ColorSpaceType dstType;
        ColorSpaceType srcType;
        ColorSpaceUtility::ConvertFunc convert;
    };
    static math::Vector3 sRGB_to_Rec709Linear(ValueType r, ValueType g, ValueType b);
    static math::Vector3 sRGB_to_Rec2020Linear(ValueType r, ValueType g, ValueType b);
    static math::Vector3 sRGB_to_XYZ(ValueType r, ValueType g, ValueType b);

    static math::Vector3 Rec709Linear_to_sRGB(ValueType r, ValueType g, ValueType b);
    static math::Vector3 Rec709Linear_to_Rec2020Linear(ValueType r, ValueType g, ValueType b);
    static math::Vector3 Rec709Linear_to_XYZ(ValueType r, ValueType g, ValueType b);

    static math::Vector3 Rec2020Linear_to_sRGB(ValueType r, ValueType g, ValueType b);
    static math::Vector3 Rec2020Linear_to_Rec709Linear(ValueType r, ValueType g, ValueType b);
    static math::Vector3 Rec2020Linear_to_XYZ(ValueType r, ValueType g, ValueType b);

    static math::Vector3 XYZ_to_sRGB(ValueType r, ValueType g, ValueType b);
    static math::Vector3 XYZ_to_Rec709Linear(ValueType r, ValueType g, ValueType b);
    static math::Vector3 XYZ_to_Rec2020Linear(ValueType r, ValueType g, ValueType b);

    static math::Vector3 Spectrum_to_XYZ(const Spectrum& spectrum);
    static math::Vector3 Spectrum_to_Rec709Linear(const Spectrum& spectrum);
    static math::Vector3 Spectrum_to_Rec2020Linear(const Spectrum& spectrum);

    static bool Convert(ValueType& dstR, ValueType& dstG, ValueType& dstB, ColorSpaceType dstType, ValueType srcR, ValueType srcG, ValueType srcB, ColorSpaceType srcType);
private:
    static const ColorSpaceConvert s_colorSpaceConverts[];
};
}
