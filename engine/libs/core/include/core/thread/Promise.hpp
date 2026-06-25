#pragma once

#include <future>

namespace slug::core
{

template<typename T>
using TPromise = std::promise<T>;

}
