#pragma once

#include <future>

namespace slug::core
{

template<typename T>
using TFuture = std::future<T>;

template<typename T>
using TSharedFuture = std::shared_future<T>;

inline constexpr std::future_status FutureReady = std::future_status::ready;
inline constexpr std::future_status FutureTimeout = std::future_status::timeout;
inline constexpr std::future_status FutureDeferred = std::future_status::deferred;

}
