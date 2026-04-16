#pragma once

#include "core/TypeConcept.hpp"
#include "core/color/ColorBase.hpp"

namespace slug::core
{
struct Color : public ColorBase 
{
    using Base = ColorBase;
    using ValueType = Base::ValueType;

    Color() 
        :Base()
    {}

    Color(const ColorBase& v0)
        :Base(v0)
    {}
        
    Color(ValueType r, ValueType g, ValueType b, ValueType a)
        :Base(r, g, b, a)
    {}

    Color(const std::initializer_list<ValueType>& list)
        :Base(list)
    {}

    Color(const Color& v0) 
    {
        *this = ColorMethod::Equal(this->colorSpace, v0);
    }

    void operator=(const Color& v0)
    {
        auto ret = ColorMethod::Equal(this->colorSpace, v0);
        this->r = ret.r;
        this->g = ret.g;
        this->b = ret.b;
        this->a = ret.a;
        this->colorSpace = ret.colorSpace;
    }

    bool operator==(const Color& v0) const
    {
        return ColorMethod::Compare(*this, v0);
    }

    Color operator+(const Color& v0) const
    {
        return ColorMethod::Add(*this, v0);
    }

    Color operator-(const Color& v0) const
    {
        return ColorMethod::Sub(*this, v0);
    }

    Color operator*(const Color& v0) const
    {
        return ColorMethod::Mul(*this, v0);
    }

    Color operator/(const Color& v0) const
    {
        return ColorMethod::Div(*this, v0);
    }

    Color operator+(ValueType v0) const
    {
        return ColorMethod::Add(*this, v0);
    }

    Color operator-(ValueType v0) const
    {
        return ColorMethod::Sub(*this, v0);
    }

    Color operator*(ValueType v0) const
    {
        return ColorMethod::Mul(*this, v0);
    }

    Color operator/(ValueType v0) const
    {
        return ColorMethod::Div(*this, v0);
    }

    void operator+=(const Color& v0)
    {
        *this = ColorMethod::Add(*this, v0);
    }

    void operator-=(const Color& v0)
    {
        *this = ColorMethod::Sub(*this, v0);
    }

    void operator*=(const Color& v0)
    {
        *this = ColorMethod::Mul(*this, v0);
    }

    void operator/=(const Color& v0)
    {
        *this = ColorMethod::Div(*this, v0);
    }

    void operator+=(ValueType v0)
    {
        *this = ColorMethod::Add(*this, v0);
    }

    void operator-=(ValueType v0)
    {
        *this = ColorMethod::Sub(*this, v0);
    }

    void operator*=(ValueType v0)
    {
        *this = ColorMethod::Div(*this, v0);
    }

    void operator/=(ValueType v0)
    {
        *this = ColorMethod::Mul(*this, v0);
    }

};
}
