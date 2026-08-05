#pragma once

#include "math/Math.hpp"

namespace slug::core
{

struct Spectrum 
{
    static constexpr uint32_t s_waveCount = 500;
    static constexpr uint32_t s_minWaveLength = 330;
    static constexpr uint32_t s_maxWaveLength = 830;
    using ValueType = float;

    ValueType data[s_waveCount] = {};

    Spectrum() 
    {
        for (size_t i = 0; i < s_waveCount; i++) 
        {
            data[i] = static_cast<ValueType>(0);
        }
    }

    bool operator==(const Spectrum& v0) const
    {
        for (size_t i = 0; i < s_waveCount; i++)
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
        for (size_t i = 0; i < s_waveCount; i++)
        {
            ret.data[i] = data[i] + v0.data[i];
        }
        return ret;
    }

    Spectrum operator-(const Spectrum& v0) const
    {
        Spectrum ret = {};
        for (size_t i = 0; i < s_waveCount; i++)
        {
            ret.data[i] = data[i] - v0.data[i];
        }
        return ret;
    }

    Spectrum operator*(const Spectrum& v0) const
    {
        Spectrum ret = {};
        for (size_t i = 0; i < s_waveCount; i++)
        {
            ret.data[i] = data[i] * v0.data[i];
        }
        return ret;
    }

    Spectrum operator/(const Spectrum& v0) const
    {
        Spectrum ret = {};
        for (size_t i = 0; i < s_waveCount; i++)
        {
            ret.data[i] = data[i] / v0.data[i];
        }
        return ret;
    }

    Spectrum operator+(ValueType v0) const
    {
        Spectrum ret = {};
        for (size_t i = 0; i < s_waveCount; i++)
        {
            ret.data[i] = data[i] / v0;
        }
        return ret;
    }

    Spectrum operator-(ValueType v0) const
    {
        Spectrum ret = {};
        for (size_t i = 0; i < s_waveCount; i++)
        {
            ret.data[i] = data[i] - v0;
        }
        return ret;
    }

    Spectrum operator*(ValueType v0) const
    {
        Spectrum ret = {};
        for (size_t i = 0; i < s_waveCount; i++)
        {
            ret.data[i] = data[i] * v0;
        }
        return ret;
    }

    Spectrum operator/(ValueType v0) const
    {
        Spectrum ret = {};
        for (size_t i = 0; i < s_waveCount; i++)
        {
            ret.data[i] = data[i] / v0;
        }
        return ret;
    }

    void operator+=(const Spectrum& v0)
    {
        for (size_t i = 0; i < s_waveCount; i++)
        {
            data[i] = data[i] + v0.data[i];
        }
    }

    void operator-=(const Spectrum& v0)
    {
        for (size_t i = 0; i < s_waveCount; i++)
        {
            data[i] = data[i] - v0.data[i];
        }
    }

    void operator*=(const Spectrum& v0)
    {
        for (size_t i = 0; i < s_waveCount; i++)
        {
            data[i] = data[i] * v0.data[i];
        }
    }

    void operator/=(const Spectrum& v0)
    {
        for (size_t i = 0; i < s_waveCount; i++)
        {
            data[i] = data[i] / v0.data[i];
        }
    }

    void operator+=(ValueType v0)
    {
        for (size_t i = 0; i < s_waveCount; i++)
        {
            data[i] = data[i] + v0;
        }
    }

    void operator-=(ValueType v0)
    {
        for (size_t i = 0; i < s_waveCount; i++)
        {
            data[i] = data[i] - v0;
        }
    }

    void operator*=(ValueType v0)
    {
        for (size_t i = 0; i < s_waveCount; i++)
        {
            data[i] = data[i] * v0;
        }
    }

    void operator/=(ValueType v0)
    {
        for (size_t i = 0; i < s_waveCount; i++)
        {
            data[i] = data[i] / v0;
        }
    }

    uint32_t GetWaveLength(size_t index) const
    {
        return static_cast<uint32_t>(index) + s_minWaveLength;
    }
};

}
