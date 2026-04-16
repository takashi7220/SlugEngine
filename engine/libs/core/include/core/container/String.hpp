#pragma once

#if defined(SLUG_USE_STL)
#include <string>
#include <string.h>
#include <cstdlib>
#include <string_view>
#include <sstream>
#endif

#include "core/memory/Allocator.hpp"
#include "core/debug/Exception.hpp"

namespace slug::core
{
#if defined(SLUG_USE_STL)
using String = std::basic_string<char, std::char_traits<char>, TAllocator<char>>;

using WString = std::basic_string<wchar_t, std::char_traits<wchar_t>, TAllocator<wchar_t>>;

using StringView = std::string_view;

using WStringView = std::wstring_view;

using StringStream = std::stringstream;

using WStringStream = std::wstringstream;
#else
#endif

template<typename T>
struct is_string : std::false_type
{
};
template<>
struct is_string<core::String> : std::true_type
{
};
template<typename T>
inline constexpr bool is_string_v = is_string<T>::value;


class StringUtility
{
public:
    static WString GetWstring(const char* str)
    {
        size_t requireSize = 0;
        size_t convertedSize = 0;
        mbstowcs_s(&requireSize, nullptr, 0, str, 0);
        if (requireSize == static_cast<size_t>(-1))
        {
            SLUG_THROW_EXCEPTION("Invalid multibyte sequence");
        }

        WString wstr(requireSize, L'\0');
        mbstowcs_s(&convertedSize, &wstr[0], requireSize, str, requireSize - 1);
        return wstr;
    }

    static String GetString(const wchar_t* wstr)
    {
        size_t requireSize = 0;
        size_t convertedSize = 0;
        wcstombs_s(&requireSize, nullptr, 0, wstr, 0);
        if (requireSize == static_cast<size_t>(-1))
        {
            SLUG_THROW_EXCEPTION("Invalid wide character sequence");
        }

        String str(requireSize, '\0');
        wcstombs_s(&convertedSize, &str[0], requireSize, wstr, requireSize - 1);
        return str;
    }

    static int32_t Strcmp(const char* v0, const char* v1)
    {
        return strcmp(v0, v1);
    }

    static void Strncpy(char* dst, const char* src, size_t count)
    {
        strncpy_s(dst, count, src, count);
    }

    template<size_t Size = 512, class... Args>
    static void Sprintfs(core::String& msg, const char* fmt, Args&&... args)
    {
        char buf[Size];

        int n = std::snprintf(buf, static_cast<int>(Size), fmt, std::forward<Args>(args)...);

        if (n < 0)
        {
            // フォーマットエラー：必要なら例外やログ
            return;
        }

        if (static_cast<std::size_t>(n) >= Size)
        {
            SLUG_THROW_EXCEPTION("Too Large Message");
        }
        else
        {
            msg.append(buf);
        }

        msg.push_back('\n');
    }

    template<size_t Size = 512, class... Args>
    static core::String Sprintfs(const char* fmt, Args&&... args)
    {
        core::String msg;
        Sprintfs<Size>(msg, fmt, std::forward<Args>(args)...);
        return msg;
    }
};

template <size_t N>
class FixedString
{
public:
    FixedString()
    {
        m_data[0] = '\0';
    }

    FixedString(const char* str)
    {
        assign(str);
    }

    FixedString(std::string_view sv)
    {
        assign(sv);
    }

    FixedString(const std::string& str)
    {
        assign(str);
    }

    FixedString& operator=(const char* str)
    {
        assign(str);
        return *this;
    }

    FixedString& operator=(std::string_view sv)
    {
        assign(sv);
        return *this;
    }

    FixedString& operator=(const std::string& str)
    {
        assign(str);
        return *this;
    }

    const char* c_str() const
    {
        return m_data;
    }
    const char* data() const
    {
        return m_data;
    }
    char* data()
    {
        return m_data;
    }

    core::StringView view() const
    {
        return core::StringView(m_data, size());
    }
    core::String to_string() const
    {
        return core::String(m_data, size());
    }

    size_t size() const
    {
        return m_size;
    }
    constexpr size_t capacity() const
    {
        return N - 1;
    }

    char operator[](size_t i) const
    {
        if (i >= N)
        {
            SLUG_THROW_EXCEPTION("FixedString: index out of range");
        }
        return m_data[i];
    }

    char& operator[](size_t i)
    {
        if (i >= N)
        {
            SLUG_THROW_EXCEPTION("FixedString: index out of range");
        }
        return m_data[i];
    }
    bool operator==(const core::FixedString<N>& rhs) const
    {
        return view() == rhs.view();
    }
    bool operator!=(const core::FixedString<N>& rhs) const
    {
        return view() != rhs.view();
    }
    bool operator==(core::StringView rhs) const
    {
        return view() == rhs;
    }
    bool operator!=(core::StringView rhs) const
    {
        return view() != rhs;
    }
private:
    void assign(core::StringView sv)
    {
        if (sv.size() >= N)
        {
            SLUG_THROW_EXCEPTION("FixedString: string too long");
        }
        std::memcpy(m_data, sv.data(), sv.size());
        m_data[sv.size()] = '\0';
        m_size = sv.size();
    }

private:
    char m_data[N] = {};
    size_t m_size = 0;
};

}
