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

// count は終端文字を含む dst のバイト数。収まらない入力は切り詰める。
// 第4引数に count を渡すと終端分が足りず、不正パラメータ扱いで異常終了する。
inline void Strncpy(char* dst, const char* src, size_t count)
{
    strncpy_s(dst, count, src, _TRUNCATE);
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
