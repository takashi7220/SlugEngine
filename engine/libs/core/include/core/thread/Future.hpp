#pragma once

#include <future>

namespace slug
{
namespace core
{

template<typename T>
using TFuture = std::future<T>;

template<typename T>
using TSharedFuture = std::shared_future<T>;


}
}
