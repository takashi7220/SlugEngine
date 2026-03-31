#pragma once

#include <functional>

namespace slug
{
namespace core
{
template<typename T>
using TFunctionObject = std::function<T>;
}
}
