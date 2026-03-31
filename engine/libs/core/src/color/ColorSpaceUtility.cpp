#include "core/color/ColorSpaceUtility.hpp"

namespace slug::core
{

struct ColorSpaceConvert
{
    ColorSpaceType dstType;
    ColorSpaceType srcType;
    ColorSpaceUtility::ConvertFunc convert;
};

static const ColorSpaceConvert C_COLORSPACE_CONVERT_TABLE[] =
{
    {ColorSpaceType::Rec709Gamma, ColorSpaceType::Rec709Linear, ColorSpaceUtility::Rec709LinearToRec709Gamma},
    {ColorSpaceType::Rec2020Linear, ColorSpaceType::Rec709Linear, ColorSpaceUtility::Rec709LinearToRec2020Linear},
    {ColorSpaceType::Rec2020PQ, ColorSpaceType::Rec709Linear, ColorSpaceUtility::Rec709LinearToRec2020PQ},
    {ColorSpaceType::sRGB, ColorSpaceType::Rec709Linear, ColorSpaceUtility::Rec709LinearToSRGB},
    {ColorSpaceType::XYZ, ColorSpaceType::Rec709Linear, ColorSpaceUtility::Rec709LinearToXYZ},
};


bool ColorSpaceUtility::Convert(ValueType& dstR, ValueType& dstG, ValueType& dstB, ColorSpaceType dstType, ValueType srcR, ValueType srcG, ValueType srcB, ColorSpaceType srcType)
{
    for (auto& c : C_COLORSPACE_CONVERT_TABLE)
    {
        if (c.dstType == dstType && c.srcType == srcType)
        {
            Vector3 ret = c.convert(srcR, srcG, srcB);
            dstR = ret.x;
            dstG = ret.y;
            dstB = ret.z;
            return true;
        }
    }
    return false;
}
}
