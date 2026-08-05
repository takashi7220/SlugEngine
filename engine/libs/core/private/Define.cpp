#pragma once

#if false
#define SLUG_ENABLE_CPP14 __cplusplus >= 201402L
#define SLUG_ENABLE_CPP17 __cplusplus >= 201703L
#define SLUG_ENABLE_CPP20 __cplusplus >= 202002L
#endif

#define SLUG_FORCE_INLINE __forceinline

#define SLUG_DEPRECATED [[deprecated]]
#define SLUG_DEPRECATED_MSG(msg) [[deprecated(msg)]]

#define SLUG_MAYBE_UNUSED [[maybe_unused]]
#define SLUG_NODISCARD [[nodiscard]]