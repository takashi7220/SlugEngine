#pragma once

#include "math/Math.hpp"

namespace slug
{
namespace math
{

class MatrixMethod
{
public:
    static size_t GetIndex(size_t x, size_t y, size_t col) 
    {
        return x * col + y;
    }

    static void GetIndex(size_t i, size_t row, size_t& x, size_t& y)
    {
        x = i / row;
        y = i % row;
    }
};
}
}
