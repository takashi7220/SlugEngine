#pragma once
#include <queue>

#include "core/container/Deque.hpp"

namespace slug
{
namespace core
{
template<typename T, typename Container = core::TDeque<T>>
using TQueue = std::queue<T, Container>;
}
}
