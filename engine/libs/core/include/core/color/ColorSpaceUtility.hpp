#pragma once

#include "math/Math.hpp"
#include "math/Vector3.hpp"
#include "math/Vector4.hpp"
#include "math/Matrix3x3.hpp"
#include "core/color/Spectrum.hpp"

namespace slug 
{
namespace core 
{

enum class ColorSpaceType
{
    Rec709Linear,
    Rec709Gamma,
    Rec2020Linear,
    Rec2020PQ,
    sRGB,
    XYZ
};

/**
 * @note https://registry.khronos.org/DataFormat/specs/1.3/dataformat.1.3.pdf
*/
class ColorSpaceUtility 
{
public:
    using ValueType = float;
    using Vector3 = math::Vector3;
    using Matrix3x3 = math::Matrix3x3;

    static constexpr ValueType ITU_OETF_ALPHA = 1.099296826809443f;
    static constexpr ValueType ITU_OETF_BETA = 0.018053968510808f;
    static constexpr ValueType ITU_OETF_GAMMA = 0.081242858298635f; //(ITU_OETF_ALPHA * ITU_OETF_BETA^0.45 - (ITU_OETF_ALPHA - 1.0))

    static constexpr ValueType SRGB_OETF_ALPHA = 1.055f;
    static constexpr ValueType SRGB_OETF_BETA = 0.04045f;
    static constexpr ValueType SRGB_OETF_GAMMA = 0.0031308f;

    static constexpr ValueType PQ_M1 = 0.1593017578125f;
    static constexpr ValueType PQ_M2 = 78.84375f;
    static constexpr ValueType PQ_C1 = 0.8359375f;
    static constexpr ValueType PQ_C2 = 18.8515625f;
    static constexpr ValueType PQ_C3 = 18.6875f;

    static constexpr ValueType HDR10_MAX_NITS = 10000.0f;
    using ConvertFunc = Vector3(*)(ValueType, ValueType, ValueType);

    static inline const Matrix3x3 REC709_TO_REC2020 = 
    {
        {0.627404f, 0.329282f, 0.043314f},
        {0.069097f, 0.919541f, 0.011362f},
        {0.016392f, 0.088013f, 0.895595f}
    };

    static inline const Matrix3x3 REC2020_TO_REC709 =
    {
        {1.660491f, -0.587641f, -0.072850f},
        {-0.124551f, 1.132900f, -0.008349f},
        {-0.018151f, -0.100579f, 1.118730f}
    };

    static inline const Matrix3x3 REC2020_TO_XYZ =
    {
        {0.636958f, 0.144617, 0.168881f},
        {0.262700f, 0.677998, 0.059302f},
        {0.000000f, 0.028073, 1.060985f}
    };

    static inline const Matrix3x3 XYZ_TO_REC2020 =
    {
        {1.716651f, -0.355671f, -0.253366f},
        {-0.666684f, 1.616481f, 0.015769f},
        {0.017640f, -0.042771f, 0.942103f}
    };

    static inline const Matrix3x3 REC709_TO_XYZ =
    {
        {0.412391f, 0.357584f, 0.180481f},
        {0.212639f, 0.715169f, 0.072192f},
        {0.019331f, 0.119195f, 0.950532f}
    };

    static inline const Matrix3x3 XYZ_TO_REC709 =
    {
        {3.240970f, -1.537383f, -0.498611f},
        {-0.969244f, 1.875968f, 0.041555f},
        {0.055630f, -0.203977f, 1.056972f}
    };

    // use BT.601, BT.709, BT.2020
    static Vector3 ITU_OETF(ValueType r, ValueType g, ValueType b) 
    {
        Vector3 ret = {};
        ret.x = r < ITU_OETF_BETA ? r * 4.5f : ITU_OETF_ALPHA * math::TPow(r, 0.45f) - (ITU_OETF_ALPHA - 1.0f);
        ret.y = g < ITU_OETF_BETA ? g * 4.5f : ITU_OETF_ALPHA * math::TPow(g, 0.45f) - (ITU_OETF_ALPHA - 1.0f);
        ret.z = b < ITU_OETF_BETA ? b * 4.5f : ITU_OETF_ALPHA * math::TPow(b, 0.45f) - (ITU_OETF_ALPHA - 1.0f);
        return ret;
    }

    // use BT.601, BT.709, BT.2020
    static Vector3 ITU_EOTF(ValueType r, ValueType g, ValueType b)
    {
        Vector3 ret = {};
        ret.x = r < ITU_OETF_GAMMA ? r / 4.5f : math::TPow((r + (ITU_OETF_ALPHA - 1.0f)) / ITU_OETF_ALPHA, 1.0f / 0.45f);
        ret.y = g < ITU_OETF_GAMMA ? g / 4.5f : math::TPow((g + (ITU_OETF_ALPHA - 1.0f)) / ITU_OETF_ALPHA, 1.0f / 0.45f);
        ret.z = b < ITU_OETF_GAMMA ? b / 4.5f : math::TPow((b + (ITU_OETF_ALPHA - 1.0f)) / ITU_OETF_ALPHA, 1.0f / 0.45f);
        return ret;
    }

    // use SRGB
    static Vector3 SRGB_OETF(ValueType r, ValueType g, ValueType b)
    {
        Vector3 ret = {};
        ret.x = r <= SRGB_OETF_GAMMA ? r * 12.92f : SRGB_OETF_ALPHA * math::TPow(r, 1.0f / 2.4f) - (SRGB_OETF_ALPHA - 1.0f);
        ret.y = g <= SRGB_OETF_GAMMA ? g * 12.92f : SRGB_OETF_ALPHA * math::TPow(g, 1.0f / 2.4f) - (SRGB_OETF_ALPHA - 1.0f);
        ret.z = b <= SRGB_OETF_GAMMA ? b * 12.92f : SRGB_OETF_ALPHA * math::TPow(b, 1.0f / 2.4f) - (SRGB_OETF_ALPHA - 1.0f);
        return ret;
    }

    // use SRGB
    static Vector3 SRGB_EOTF(ValueType r, ValueType g, ValueType b)
    {
        Vector3 ret = {};
        ret.x = r <= SRGB_OETF_BETA ? r / 12.92f : math::TPow((r + (SRGB_OETF_ALPHA - 1.0f)) / SRGB_OETF_ALPHA, 2.4f);
        ret.y = g <= SRGB_OETF_BETA ? g / 12.92f : math::TPow((g + (SRGB_OETF_ALPHA - 1.0f)) / SRGB_OETF_ALPHA, 2.4f);
        ret.z = b <= SRGB_OETF_BETA ? b / 12.92f : math::TPow((b + (SRGB_OETF_ALPHA - 1.0f)) / SRGB_OETF_ALPHA, 2.4f);
        return ret;
    }

    static Vector3 GAMMA_2_EOTF(ValueType r, ValueType g, ValueType b)
    {
        Vector3 ret = {};
        ret.x = math::TPow(r, 2.0f);
        ret.y = math::TPow(g, 2.0f);
        ret.z = math::TPow(b, 2.0f);
        return ret;
    }

    static Vector3 GAMMA_2_OETF(ValueType r, ValueType g, ValueType b)
    {
        Vector3 ret = {};
        ret.x = math::TPow(r, 1.0f / 2.0f);
        ret.y = math::TPow(g, 1.0f / 2.0f);
        ret.z = math::TPow(b, 1.0f / 2.0f);
        return ret;
    }

    static Vector3 GAMMA_22_EOTF(ValueType r, ValueType g, ValueType b)
    {
        Vector3 ret = {};
        ret.x = math::TPow(r, 2.2f);
        ret.y = math::TPow(g, 2.2f);
        ret.z = math::TPow(b, 2.2f);
        return ret;
    }

    static Vector3 GAMMA_22_OETF(ValueType r, ValueType g, ValueType b)
    {
        Vector3 ret = {};
        ret.x = math::TPow(r, 1.0f / 2.2f);
        ret.y = math::TPow(g, 1.0f / 2.2f);
        ret.z = math::TPow(b, 1.0f / 2.2f);
        return ret;
    }

    static Vector3 NormalizeNits(ValueType r, ValueType g, ValueType b, ValueType baseNits = 250.0f)
    {
        return Vector3(r, g, b) * math::TMax(baseNits, 1.0f) / HDR10_MAX_NITS;
    }

    static Vector3 InverseNormalizeNits(ValueType r, ValueType g, ValueType b, ValueType baseNits = 250.0f)
    {
        return Vector3(r, g, b) * HDR10_MAX_NITS / math::TMax(baseNits, 1.0f);
    }

    static ValueType PQ_EOTF(ValueType v)
    {
        ValueType tmp0 = math::TMax((math::TPow(v, 1.0f / PQ_M2) - PQ_C1), 0.0f);
        ValueType tmp1 = PQ_C2 - PQ_C3 * math::TPow(v, 1.0f / PQ_M2);
        return math::TPow(tmp0 / tmp1, 1.0f / PQ_M1);
    }

    static Vector3 PQ_EOTF(ValueType r, ValueType g, ValueType b)
    {
        return Vector3(PQ_EOTF(r), PQ_EOTF(g), PQ_EOTF(b));
    }

    static Vector3 PQ_EOTF_NORMALIZE(ValueType r, ValueType g, ValueType b, ValueType baseNits = 250.0f)
    {
        Vector3 ret = { r, g ,b };
        ret = InverseNormalizeNits(r, g, b, baseNits);
        return PQ_EOTF(ret.x, ret.y, ret.z);
    }

    static ValueType PQ_OETF(ValueType v)
    {
        ValueType tmp0 = PQ_C1 + PQ_C2 * math::TPow(v, PQ_M1);
        ValueType tmp1 = 1.0f + PQ_C3 * math::TPow(v, PQ_M1);
        return math::TPow(tmp0 / tmp1, PQ_M2);
    }

    static Vector3 PQ_OETF(ValueType r, ValueType g, ValueType b)
    {
        return Vector3(PQ_OETF(r), PQ_OETF(g), PQ_OETF(b));
    }

    static Vector3 PQ_OETF_NORMALIZE(ValueType r, ValueType g, ValueType b)
    {
        ValueType baseNits = 250.0f;
        Vector3 ret = { r, g ,b };
        ret = NormalizeNits(r, g, b, baseNits);
        return PQ_OETF(ret.x, ret.y, ret.z);
    }

    static Vector3 Rec709toRec2020(ValueType r, ValueType g, ValueType b)
    {
        return REC709_TO_REC2020 * Vector3(r, g, b);

    }

    static Vector3 Rec2020toRec709(ValueType r, ValueType g, ValueType b)
    {
        return REC2020_TO_REC709 * Vector3(r, g, b);
    }

    static Vector3 Rec709toRecXYZ(ValueType r, ValueType g, ValueType b)
    {
        return REC709_TO_XYZ * Vector3(r, g, b);

    }

    static Vector3 XYZtoRec709(ValueType r, ValueType g, ValueType b)
    {
        return XYZ_TO_REC709 * Vector3(r, g, b);
    }

    static Vector3 XYZtoRec2020(ValueType r, ValueType g, ValueType b)
    {
        return XYZ_TO_REC2020 * Vector3(r, g, b);

    }

    static Vector3 Rec2020toXYZ(ValueType r, ValueType g, ValueType b)
    {
        return REC2020_TO_XYZ * Vector3(r, g, b);
    }

    static Vector3 Rec709LinearToRec709Gamma(ValueType r, ValueType g, ValueType b)
    {
        return GAMMA_22_OETF(r, g, b);
    }

    static Vector3 Rec709LinearToSRGB(ValueType r, ValueType g, ValueType b)
    {
        return GAMMA_22_OETF(r, g, b);
    }

    static Vector3 Rec709LinearToRec2020Linear(ValueType r, ValueType g, ValueType b)
    {
        return Rec2020toRec709(r, g, b);
    }

    static Vector3 Rec709LinearToRec2020PQ(ValueType r, ValueType g, ValueType b)
    {
        auto ret = Rec2020toRec709(r, g, b);
        return PQ_OETF(ret.x ,ret.y, ret.z);
    }

    static Vector3 Rec709LinearToXYZ(ValueType r, ValueType g, ValueType b)
    {
        return Rec709toRecXYZ(r, g, b);
    }

    // https://jcgt.org/published/0002/02/01/
    // Simple Analytic Approximations to the CIE XYZ Color Matching Functions by Nvidia
    static ValueType CombineGaussianCurve(ValueType x, ValueType myu, ValueType sigma0, ValueType sigma1) 
    {
        if (x < myu) 
        {
            ValueType t0 = -0.5f * math::TPow(x - myu, 2.0f);
            ValueType t1 = sigma0 * sigma0;
            return t1 > 0 ? math::TExp(t0 / t1) : 0;
        }
        else 
        {
            ValueType t0 = -0.5f * math::TPow(x - myu, 2.0f);
            ValueType t1 = sigma1 * sigma1;
            return t1 > 0 ? math::TExp(t0 / t1) : 0;
        }
    }

    // https://jcgt.org/published/0002/02/01/
    // Simple Analytic Approximations to the CIE XYZ Color Matching Functions by Nvidia
    static Vector3 CIE1931_XYZ_CMF(ValueType wavelength) 
    {
        Vector3 ret = {};
        ret.x = 1.056f * CombineGaussianCurve(wavelength, 599.8f, 37.9f, 31.0f)
                + 0.362f * CombineGaussianCurve(wavelength, 442.0f, 16.0f, 26.7f)
                - 0.065f * CombineGaussianCurve(wavelength, 501.1f, 20.4f, 26.2f);

        ret.y = 0.821f * CombineGaussianCurve(wavelength, 568.8f, 46.9f, 40.5f)
                + 0.286f * CombineGaussianCurve(wavelength, 530.9f, 16.3f, 31.1f);

        ret.z = 1.217f * CombineGaussianCurve(wavelength, 437.0f, 11.8f, 36.0f)
                + 0.681f * CombineGaussianCurve(wavelength, 459.0f, 26.0f, 13.8f);
    }

    static Vector3 SpectrumtoXYZ(const Spectrum& m) 
    {
        Vector3 ret = {};
        for (size_t i = 0; i < m.waveCount; i++)
        {
            uint32_t wavelength = m.GetWaveLength(i);
            Vector3 xyz =  CIE1931_XYZ_CMF(static_cast<ValueType>(wavelength));
            ret.x += xyz.x * m.data[i];
            ret.y += xyz.y * m.data[i];
            ret.z += xyz.z * m.data[i];
        }
        return ret;
    }

    static bool Convert(ValueType& dstR, ValueType& dstG, ValueType& dstB, ColorSpaceType dstType, ValueType srcR, ValueType srcG, ValueType srcB, ColorSpaceType srcType);

};
}
}
