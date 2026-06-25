#include "core/color/ColorSpaceUtility.hpp"

namespace slug::core
{

const ColorSpaceUtility::ColorSpaceConvert ColorSpaceUtility::s_colorSpaceConverts[] =
{
    { ColorSpaceType::sRGB, ColorSpaceType::Rec709Linear, ColorSpaceUtility::Rec709Linear_to_sRGB },
    { ColorSpaceType::sRGB, ColorSpaceType::Rec2020Linear, ColorSpaceUtility::Rec2020Linear_to_sRGB },
    { ColorSpaceType::sRGB, ColorSpaceType::XYZ, ColorSpaceUtility::XYZ_to_sRGB },
    { ColorSpaceType::Rec709Linear, ColorSpaceType::sRGB, ColorSpaceUtility::sRGB_to_Rec709Linear },
    { ColorSpaceType::Rec709Linear, ColorSpaceType::Rec2020Linear, ColorSpaceUtility::Rec2020Linear_to_Rec709Linear },
    { ColorSpaceType::Rec709Linear, ColorSpaceType::XYZ, ColorSpaceUtility::XYZ_to_Rec709Linear },
    { ColorSpaceType::Rec2020Linear, ColorSpaceType::sRGB, ColorSpaceUtility::sRGB_to_Rec2020Linear },
    { ColorSpaceType::Rec2020Linear, ColorSpaceType::Rec709Linear, ColorSpaceUtility::Rec709Linear_to_Rec2020Linear },
    { ColorSpaceType::Rec2020Linear, ColorSpaceType::XYZ, ColorSpaceUtility::XYZ_to_Rec2020Linear },
    { ColorSpaceType::XYZ, ColorSpaceType::sRGB, ColorSpaceUtility::sRGB_to_XYZ },
    { ColorSpaceType::XYZ, ColorSpaceType::Rec709Linear, ColorSpaceUtility::Rec709Linear_to_XYZ },
    { ColorSpaceType::XYZ, ColorSpaceType::Rec2020Linear, ColorSpaceUtility::Rec2020Linear_to_XYZ },
};

/**
 * @brief https://registry.khronos.org/DataFormat/specs/1.3/dataformat.1.3.pdf
 */
struct ColorSpaceDataBase
{
    static inline const math::Matrix3x3 REC709_TO_REC2020 =
    {
        {0.627404f, 0.329282f, 0.043314f},
        {0.069097f, 0.919541f, 0.011362f},
        {0.016392f, 0.088013f, 0.895595f}
    };

    static inline const math::Matrix3x3 REC2020_TO_REC709 =
    {
        {1.660491f, -0.587641f, -0.072850f},
        {-0.124551f, 1.132900f, -0.008349f},
        {-0.018151f, -0.100579f, 1.118730f}
    };

    static inline const math::Matrix3x3 REC2020_TO_XYZ =
    {
        {0.636958f, 0.144617, 0.168881f},
        {0.262700f, 0.677998, 0.059302f},
        {0.000000f, 0.028073, 1.060985f}
    };

    static inline const math::Matrix3x3 XYZ_TO_REC2020 =
    {
        {1.716651f, -0.355671f, -0.253366f},
        {-0.666684f, 1.616481f, 0.015769f},
        {0.017640f, -0.042771f, 0.942103f}
    };

    static inline const math::Matrix3x3 REC709_TO_XYZ =
    {
        {0.412391f, 0.357584f, 0.180481f},
        {0.212639f, 0.715169f, 0.072192f},
        {0.019331f, 0.119195f, 0.950532f}
    };

    static inline const math::Matrix3x3 XYZ_TO_REC709 =
    {
        {3.240970f, -1.537383f, -0.498611f},
        {-0.969244f, 1.875968f, 0.041555f},
        {0.055630f, -0.203977f, 1.056972f}
    };

    static ColorSpaceUtility::ValueType sRGB_EOTF(ColorSpaceUtility::ValueType x)
    {
        x = math::TClamp(x, 0.0f, 1.0f);
        if (x <= 0.04045f)
        {
            return x / 12.92f;
        }
        else
        {
            return math::TPow((x + 0.055f) / 1.055f, 2.4f);
        }
    }

    static ColorSpaceUtility::ValueType sRGB_OETF(ColorSpaceUtility::ValueType x)
    {
        if (x <= 0.0031308f)
        {
            return x * 12.92f;
        }
        else
        {
            return math::TPow(x, 1.0f / 2.4f) * 1.055f - 0.055f;
        }
    }


    // https://jcgt.org/published/0002/02/01/
    // Simple Analytic Approximations to the CIE XYZ Color Matching Functions by Nvidia
    static ColorSpaceUtility::ValueType CombineGaussianCurve(ColorSpaceUtility::ValueType x, ColorSpaceUtility::ValueType myu, ColorSpaceUtility::ValueType sigma0, ColorSpaceUtility::ValueType sigma1)
    {
        if (x < myu)
        {
            ColorSpaceUtility::ValueType t0 = -0.5f * math::TPow(x - myu, 2.0f);
            ColorSpaceUtility::ValueType t1 = sigma0 * sigma0;
            return t1 > 0 ? math::TExp(t0 / t1) : 0;
        }
        else
        {
            ColorSpaceUtility::ValueType t0 = -0.5f * math::TPow(x - myu, 2.0f);
            ColorSpaceUtility::ValueType t1 = sigma1 * sigma1;
            return t1 > 0 ? math::TExp(t0 / t1) : 0;
        }
    }

    // https://jcgt.org/published/0002/02/01/
    // Simple Analytic Approximations to the CIE XYZ Color Matching Functions by Nvidia
    static math::Vector3 CIE1931_XYZ_CMF(ColorSpaceUtility::ValueType wavelength)
    {
        math::Vector3 ret = {};
        ret.x = 1.056f * CombineGaussianCurve(wavelength, 599.8f, 37.9f, 31.0f)
            + 0.362f * CombineGaussianCurve(wavelength, 442.0f, 16.0f, 26.7f)
            - 0.065f * CombineGaussianCurve(wavelength, 501.1f, 20.4f, 26.2f);

        ret.y = 0.821f * CombineGaussianCurve(wavelength, 568.8f, 46.9f, 40.5f)
            + 0.286f * CombineGaussianCurve(wavelength, 530.9f, 16.3f, 31.1f);

        ret.z = 1.217f * CombineGaussianCurve(wavelength, 437.0f, 11.8f, 36.0f)
            + 0.681f * CombineGaussianCurve(wavelength, 459.0f, 26.0f, 13.8f);
        return ret;
    }

};

math::Vector3 ColorSpaceUtility::sRGB_to_Rec709Linear(ValueType r, ValueType g, ValueType b)
{
    r = ColorSpaceDataBase::sRGB_EOTF(r);
    g = ColorSpaceDataBase::sRGB_EOTF(g);
    b = ColorSpaceDataBase::sRGB_EOTF(b);
    return math::Vector3(r, g, b);
}

math::Vector3 ColorSpaceUtility::sRGB_to_Rec2020Linear(ValueType r, ValueType g, ValueType b)
{
    r = ColorSpaceDataBase::sRGB_EOTF(r);
    g = ColorSpaceDataBase::sRGB_EOTF(g);
    b = ColorSpaceDataBase::sRGB_EOTF(b);
    return Rec709Linear_to_Rec2020Linear(r, g, b);
}

math::Vector3 ColorSpaceUtility::sRGB_to_XYZ(ValueType r, ValueType g, ValueType b)
{
    r = ColorSpaceDataBase::sRGB_EOTF(r);
    g = ColorSpaceDataBase::sRGB_EOTF(g);
    b = ColorSpaceDataBase::sRGB_EOTF(b);
    return Rec709Linear_to_XYZ(r, g, b);
}

math::Vector3 ColorSpaceUtility::Rec709Linear_to_sRGB(ValueType r, ValueType g, ValueType b)
{
    r = ColorSpaceDataBase::sRGB_OETF(r);
    g = ColorSpaceDataBase::sRGB_OETF(g);
    b = ColorSpaceDataBase::sRGB_OETF(b);
    return math::Vector3(r, g, b);
}

math::Vector3 ColorSpaceUtility::Rec709Linear_to_Rec2020Linear(ValueType r, ValueType g, ValueType b)
{
    return ColorSpaceDataBase::REC709_TO_REC2020 * math::Vector3(r, g, b);
}

math::Vector3 ColorSpaceUtility::Rec709Linear_to_XYZ(ValueType r, ValueType g, ValueType b)
{
    return ColorSpaceDataBase::REC709_TO_XYZ * math::Vector3(r, g, b);
}

math::Vector3 ColorSpaceUtility::Rec2020Linear_to_sRGB(ValueType r, ValueType g, ValueType b)
{
    math::Vector3 tmp = ColorSpaceDataBase::REC2020_TO_REC709 * math::Vector3(r, g, b);
    return Rec709Linear_to_sRGB(tmp.x, tmp.y, tmp.z);
}

math::Vector3 ColorSpaceUtility::Rec2020Linear_to_Rec709Linear(ValueType r, ValueType g, ValueType b)
{
    return ColorSpaceDataBase::REC2020_TO_REC709 * math::Vector3(r, g, b);
}

math::Vector3 ColorSpaceUtility::Rec2020Linear_to_XYZ(ValueType r, ValueType g, ValueType b)
{
    return ColorSpaceDataBase::REC2020_TO_XYZ * math::Vector3(r, g, b);
}

math::Vector3 ColorSpaceUtility::XYZ_to_sRGB(ValueType r, ValueType g, ValueType b)
{
    math::Vector3 tmp = ColorSpaceDataBase::XYZ_TO_REC709 * math::Vector3(r, g, b);
    return Rec709Linear_to_sRGB(tmp.x, tmp.y, tmp.z);
}

math::Vector3 ColorSpaceUtility::XYZ_to_Rec709Linear(ValueType r, ValueType g, ValueType b)
{
    return ColorSpaceDataBase::XYZ_TO_REC709 * math::Vector3(r, g, b);
}

math::Vector3 ColorSpaceUtility::XYZ_to_Rec2020Linear(ValueType r, ValueType g, ValueType b)
{
    return ColorSpaceDataBase::XYZ_TO_REC2020 * math::Vector3(r, g, b);
}

math::Vector3 ColorSpaceUtility::Spectrum_to_XYZ(const Spectrum& spectrum)
{
    math::Vector3 ret = {};
    for (size_t i = 0; i < spectrum.s_waveCount; i++)
    {
        uint32_t wavelength = spectrum.GetWaveLength(i);
        math::Vector3 xyz = ColorSpaceDataBase::CIE1931_XYZ_CMF(static_cast<ValueType>(wavelength));
        ret.x += xyz.x * spectrum.data[i];
        ret.y += xyz.y * spectrum.data[i];
        ret.z += xyz.z * spectrum.data[i];
    }
    return ret;
}

math::Vector3 ColorSpaceUtility::Spectrum_to_Rec709Linear(const Spectrum& spectrum)
{
    math::Vector3 tmp = Spectrum_to_XYZ(spectrum);
    return ColorSpaceDataBase::XYZ_TO_REC709 * tmp;
}

math::Vector3 ColorSpaceUtility::Spectrum_to_Rec2020Linear(const Spectrum& spectrum)
{
    math::Vector3 tmp = Spectrum_to_XYZ(spectrum);
    return ColorSpaceDataBase::XYZ_TO_REC2020 * tmp;
}

bool ColorSpaceUtility::Convert(ValueType& dstR, ValueType& dstG, ValueType& dstB, ColorSpaceType dstType, ValueType srcR, ValueType srcG, ValueType srcB, ColorSpaceType srcType)
{
    for (auto& c : s_colorSpaceConverts)
    {
        if (c.dstType == dstType && c.srcType == srcType)
        {
            math::Vector3 ret = c.convert(srcR, srcG, srcB);
            dstR = ret.x;
            dstG = ret.y;
            dstB = ret.z;
            return true;
        }
    }
    return false;
}
}
