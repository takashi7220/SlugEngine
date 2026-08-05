#pragma once

#if SLUG_USE_STL
#include <mutex>
#endif

namespace slug::core
{
using Mutex = std::mutex;

template<typename T>
using LockGuard = std::lock_guard<T>;

template<typename T>
using UniqueLock = std::unique_lock<T>;

template<typename T>
using ScopedLock = std::scoped_lock<T>;
}
