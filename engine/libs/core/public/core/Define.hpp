#pragma once

#if false
#define SLUG_ENABLE_CPP14 __cplusplus >= 201402L
#define SLUG_ENABLE_CPP17 __cplusplus >= 201703L
#define SLUG_ENABLE_CPP20 __cplusplus >= 202002L
#else
#define SLUG_ENABLE_CPP14
#define SLUG_ENABLE_CPP17
#define SLUG_ENABLE_CPP20
#endif

#define SLUG_FORCE_INLINE __forceinline

#define SLUG_DEPRECATED [[deprecated]]
#define SLUG_DEPRECATED_MSG(msg) [[deprecated(msg)]]

#define SLUG_MAYBE_UNUSED [[maybe_unused]]
#define SLUG_NODISCARD [[nodiscard]]

#define SLUG_PROPERTY(...)
#define SLUG_NAMEOF(x) #x

#define SLUG_DEBUG_MODE DEBUG_MODE
#define SLUG_ENABLE_MEMORY_COUNTER SLUG_DEBUG_MODE

#define SLUG_COPY_DELETE(className) \
    className(const className&) = delete; \
    className& operator=(const className&) = delete;

#define SLUG_COPY_DEFAULT(className) \
    className(const className&) = default; \
    className& operator=(const className&) = default;
