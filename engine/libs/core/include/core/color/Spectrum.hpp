#pragma once

#include "math/Math.hpp"

namespace slug 
{
namespace core 
{

struct Spectrum 
{
    using ValueType = float;

    ValueType data[500] = {};
    uint32_t minWavelength = 330;
    uint32_t maxWavelength = 830;
    size_t waveCount = 500;

    Spectrum() 
    {
        for (size_t i = 0; i < waveCount; i++) 
        {
            data[i] = static_cast<ValueType>(0);
        }
    }

    bool operator==(const Spectrum& v0) const
    {
        for (size_t i = 0; i < waveCount; i++)
        {
            if (!math::TEqual(v0.data[i], data[i]))
            {
                return false;
            }
        }
        return true;
    }

    Spectrum operator+(const Spectrum& v0) const
    {
        Spectrum ret = {};
        for (size_t i = 0; i < waveCount; i++)
        {
            ret.data[i] = data[i] + v0.data[i];
        }
        return ret;
    }

    Spectrum operator-(const Spectrum& v0) const
    {
        Spectrum ret = {};
        for (size_t i = 0; i < waveCount; i++)
        {
            ret.data[i] = data[i] - v0.data[i];
        }
        return ret;
    }

    Spectrum operator*(const Spectrum& v0) const
    {
        Spectrum ret = {};
        for (size_t i = 0; i < waveCount; i++)
        {
            ret.data[i] = data[i] * v0.data[i];
        }
        return ret;
    }

    Spectrum operator/(const Spectrum& v0) const
    {
        Spectrum ret = {};
        for (size_t i = 0; i < waveCount; i++)
        {
            ret.data[i] = data[i] / v0.data[i];
        }
        return ret;
    }

    Spectrum operator+(ValueType v0) const
    {
        Spectrum ret = {};
        for (size_t i = 0; i < waveCount; i++)
        {
            ret.data[i] = data[i] / v0;
        }
        return ret;
    }

    Spectrum operator-(ValueType v0) const
    {
        Spectrum ret = {};
        for (size_t i = 0; i < waveCount; i++)
        {
            ret.data[i] = data[i] - v0;
        }
        return ret;
    }

    Spectrum operator*(ValueType v0) const
    {
        Spectrum ret = {};
        for (size_t i = 0; i < waveCount; i++)
        {
            ret.data[i] = data[i] * v0;
        }
        return ret;
    }

    Spectrum operator/(ValueType v0) const
    {
        Spectrum ret = {};
        for (size_t i = 0; i < waveCount; i++)
        {
            ret.data[i] = data[i] / v0;
        }
        return ret;
    }

    void operator+=(const Spectrum& v0)
    {
        for (size_t i = 0; i < waveCount; i++)
        {
            data[i] = data[i] + v0.data[i];
        }
    }

    void operator-=(const Spectrum& v0)
    {
        for (size_t i = 0; i < waveCount; i++)
        {
            data[i] = data[i] - v0.data[i];
        }
    }

    void operator*=(const Spectrum& v0)
    {
        for (size_t i = 0; i < waveCount; i++)
        {
            data[i] = data[i] * v0.data[i];
        }
    }

    void operator/=(const Spectrum& v0)
    {
        for (size_t i = 0; i < waveCount; i++)
        {
            data[i] = data[i] / v0.data[i];
        }
    }

    void operator+=(ValueType v0)
    {
        for (size_t i = 0; i < waveCount; i++)
        {
            data[i] = data[i] + v0;
        }
    }

    void operator-=(ValueType v0)
    {
        for (size_t i = 0; i < waveCount; i++)
        {
            data[i] = data[i] - v0;
        }
    }

    void operator*=(ValueType v0)
    {
        for (size_t i = 0; i < waveCount; i++)
        {
            data[i] = data[i] * v0;
        }
    }

    void operator/=(ValueType v0)
    {
        for (size_t i = 0; i < waveCount; i++)
        {
            data[i] = data[i] / v0;
        }
    }

    uint32_t GetWaveLength(size_t index) const
    {
        return static_cast<uint32_t>(index) + minWavelength;
    }
};

}
}
