#pragma once

#if SLUG_USE_STL
#include <memory>
#endif

namespace slug 
{
namespace core 
{
#if SLUG_USE_STL
template<typename T>
using TUniquePtr = std::unique_ptr<T>;

template<typename T>
inline TUniquePtr<T> MakeUnique()
{
    return std::make_unique<T>();
}

template<typename T, typename... Args>
TUniquePtr<T> MakeUnique(Args&... args)
{
    return std::make_unique<T>(args ...);
}

#endif
}
}
