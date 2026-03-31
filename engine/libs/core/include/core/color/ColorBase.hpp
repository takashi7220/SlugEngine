#pragma once

#include "core/TypeConcept.hpp"
#include "core/debug/Assert.hpp"
#include "math/Vector3.hpp"
#include "core/color/ColorSpaceUtility.hpp"
namespace slug
{
namespace core 
{

struct ColorBase
{
    using ValueType = ColorSpaceUtility::ValueType;

    ValueType r;
    ValueType g;
    ValueType b;
    ValueType a;

    ColorSpaceType colorSpace = ColorSpaceType::sRGB;
   
    ColorBase()
        : r(0)
        , g(0)
        , b(0)
        , a(0)
    {}

    ColorBase(ValueType r, ValueType g, ValueType b, ValueType a)
        : r(r)
        , g(g)
        , b(b)
        , a(a)
    {}

    ColorBase(const std::initializer_list<ValueType>& list)
        : r(0)
        , g(0)
        , b(0)
        , a(0)
    {
        auto itr = list.begin();
        if (list.size() >= 4)
        {
            r = *itr++;
            g = *itr++;
            b = *itr++;
            a = *itr++;
        }        
    }

    ColorBase(const math::Vector3& v0)
        : r(0)
        , g(0)
        , b(0)
        , a(0)
    {
        r = v0.x;
        g = v0.y;
        b = v0.z;
    }

    void operator=(const math::Vector3& v0) 
    {
        r = v0.x;
        g = v0.y;
        b = v0.z;
    }    
};

class ColorMethod
{
public:

    using ValueType = ColorSpaceUtility::ValueType;

    // 等価比較、四則演算
    static bool Compare(const ColorBase& v0, const ColorBase& v1)
    {
        if (v0.colorSpace != v1.colorSpace) 
        {
            SLUG_ASSERT_MSG(false, "The color spaces are different.");
            return false;
        }

        return v0.r == v1.r && v0.g == v1.g && v0.b == v1.b && v0.a == v1.a;
    }

    static ColorBase Add(const ColorBase& v0, const ColorBase& v1)
    {
        ColorBase ret = {};
        if (v0.colorSpace != v1.colorSpace)
        {
            SLUG_ASSERT_MSG(false, "The color spaces are different.");
            return ret;
        }

        ret.r = v0.r + v1.r;
        ret.g = v0.g + v1.g;
        ret.b = v0.b + v1.b;
        ret.a = v0.a + v1.a;
        return ret;
    }

    static ColorBase Sub(const ColorBase& v0, const ColorBase& v1)
    {
        ColorBase ret = {};
        if (v0.colorSpace != v1.colorSpace)
        {
            SLUG_ASSERT_MSG(false, "The color spaces are different.");
            return ret;
        }

        ret.r = v0.r - v1.r;
        ret.g = v0.g - v1.g;
        ret.b = v0.b - v1.b;
        ret.a = v0.a - v1.a;
        return ret;
    }


    static ColorBase Mul(const ColorBase& v0, const ColorBase& v1)
    {
        ColorBase ret = {};
        if (v0.colorSpace != v1.colorSpace)
        {
            SLUG_ASSERT_MSG(false, "The color spaces are different.");
            return ret;
        }

        ret.r = v0.r * v1.r;
        ret.g = v0.g * v1.g;
        ret.b = v0.b * v1.b;
        ret.a = v0.a * v1.a;
        return ret;
    }


    static ColorBase Div(const ColorBase& v0, const ColorBase& v1)
    {
        ColorBase ret = {};
        if (v0.colorSpace != v1.colorSpace)
        {
            SLUG_ASSERT_MSG(false, "The color spaces are different.");
            return ret;
        }

        ret.r = v0.r / v1.r;
        ret.g = v0.g / v1.g;
        ret.b = v0.b / v1.b;
        ret.a = v0.a / v1.a;
        return ret;
    }

    static ColorBase Add(const ColorBase& v0, ValueType v1)
    {
        ColorBase ret = {};
        ret.r = v0.r + v1;
        ret.g = v0.g + v1;
        ret.b = v0.b + v1;
        ret.a = v0.a + v1;
        return ret;
    }

    static ColorBase Sub(const ColorBase& v0, ValueType v1)
    {
        ColorBase ret = {};
        ret.r = v0.r - v1;
        ret.g = v0.g - v1;
        ret.b = v0.b - v1;
        ret.a = v0.a - v1;
        return ret;
    }

    static ColorBase Mul(const ColorBase& v0, ValueType v1)
    {
        ColorBase ret = {};
        ret.r = v0.r * v1;
        ret.g = v0.g * v1;
        ret.b = v0.b * v1;
        ret.a = v0.a * v1;
        return ret;
    }

    static ColorBase Div(const ColorBase& v0, ValueType v1)
    {
        ColorBase ret = {};
        ret.r = v0.r / v1;
        ret.g = v0.g / v1;
        ret.b = v0.b / v1;
        ret.a = v0.a / v1;
        return ret;
    }
public:
    static ColorBase Equal(ColorSpaceType dstColorType, const ColorBase& v0)
    {
        bool success = false;

        ColorBase ret = {};
        ret.colorSpace = dstColorType;

        ColorSpaceType srcColorType = v0.colorSpace;
        if (dstColorType == srcColorType)
        {
            ret.r = v0.r;
            ret.g = v0.g;
            ret.b = v0.b;
            ret.a = v0.a;
            success = true;
        }
        else 
        {
            success = ColorSpaceUtility::Convert(ret.r, ret.g, ret.b, ret.colorSpace, v0.r, v0.g, v0.b, v0.colorSpace);
        }

        if(!success)
        {
            SLUG_ASSERT_MSG(false, "Conversion between these color spaces is not supported.");
            ret.r = v0.r;
            ret.g = v0.g;
            ret.b = v0.b;
            ret.a = v0.a;
        }

        return ret;
    }
};

}
}
