#pragma once

#include <string.h>

namespace slug::core
{
namespace StringUtility
{
inline void Strcpy(char* dst, const char* src)
{
    strcpy_s(dst, strlen(src) + 1, src);
}

inline void Strncpy(char* dst, const char* src, size_t count)
{
    strncpy_s(dst, count, src, count);
}

inline int Strcmp(const char* v0, const char* v1)
{
    return strcmp(v0, v1);
}

inline bool IsEqual(const char* v0, const char* v1)
{
    return strcmp(v0, v1) == 0;
}

}
}
