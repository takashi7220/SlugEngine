#pragma once

#include <functional>

namespace slug::core
{
template<typename T>
using TFunctionObject = std::function<T>;
}
