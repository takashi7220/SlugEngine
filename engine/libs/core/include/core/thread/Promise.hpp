#pragma once

#include <future>

namespace slug
{
namespace core
{

template<typename T>
using TPromise = std::promise<T>;

}
}
