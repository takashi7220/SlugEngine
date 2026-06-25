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
#include "core/container/Vector.hpp"
#include "core/utility/StringUtility.hpp"

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


namespace StringUtility
{
template<size_t Size = 512, class... Args>
inline void Snprintf(core::String& msg, const char* fmt, Args&&... args) noexcept
{
    char buf[Size];
    const int n = std::snprintf(buf, Size, fmt, std::forward<Args>(args)...);
    if (n <= 0 || static_cast<size_t>(n) >= Size)
    {
        return;
    }
    msg.append(buf, static_cast<size_t>(n));
}

template<size_t Size = 512, class... Args>
inline core::String Snprintf(const char* fmt, Args&&... args) noexcept
{
    core::String msg;
    Snprintf<Size>(msg, fmt, std::forward<Args>(args)...);
    return msg;
}

inline String ToString(int32_t value)
{
    core::String str = {};
    Snprintf(str, "%d", value);
    return str;
}

inline String ToString(uint32_t value)
{
    core::String str = {};
    Snprintf(str, "%u", value);
    return str;
}

inline String ToString(int64_t value)
{
    core::String str = {};
    Snprintf(str, "%lld", value);
    return str;
}

inline String ToString(uint64_t value)
{
    core::String str = {};
    Snprintf(str, "%lld", value);
    return str;
}

inline String ToString(float value)
{
    core::String str = {};
    Snprintf(str, "%f", value);
    return str;
}

inline String ToString(double value)
{
    core::String str = {};
    Snprintf(str, "%f", value);
    return str;
}

inline WString GetWstring(const char* str)
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

inline String GetString(WStringView wstr)
{
    size_t requireSize = 0;
    size_t convertedSize = 0;
    wcstombs_s(&requireSize, nullptr, 0, wstr.data(), 0);
    if (requireSize == static_cast<size_t>(-1))
    {
        SLUG_THROW_EXCEPTION("Invalid wide character sequence");
    }

    String str(requireSize, '\0');
    wcstombs_s(&convertedSize, &str[0], requireSize, wstr.data(), requireSize - 1);
    return str;
}

inline core::TVector<core::String> Split(const core::String& str, const core::String& delimiter)
{
    core::TVector<core::String> tokens;
    size_t start = 0;
    size_t end = str.find(delimiter);
    while (end != core::String::npos)
    {
        tokens.push_back(str.substr(start, end - start));
        start = end + delimiter.length();
        end = str.find(delimiter, start);
    }
    tokens.push_back(str.substr(start));
    return tokens;
}

inline void Split(const core::String& str, const core::String& delimiter, core::TVector<core::String>& tokens)
{
    size_t start = 0;
    size_t end = str.find(delimiter);
    while (end != core::String::npos)
    {
        tokens.push_back(str.substr(start, end - start));
        start = end + delimiter.length();
        end = str.find(delimiter, start);
    }
    tokens.push_back(str.substr(start));
}

inline size_t Hash(core::StringView view)
{
    const uint64_t fnv_offset_basis = 14695981039346656037ull;
    const uint64_t fnv_prime = 1099511628211ull;
    uint64_t hash = fnv_offset_basis;
    for (char c : view)
    {
        hash ^= static_cast<uint8_t>(c);
        hash *= fnv_prime;
    }
    return static_cast<size_t>(hash);
}

inline uint32_t Stou(core::StringView view)
{
    return static_cast<uint32_t>(std::stoul(view.data()));
}

inline int32_t Stoi(core::StringView view)
{
    return std::stoi(view.data());
}

inline float Stof(core::StringView view)
{
    return std::stof(view.data());
}

inline double Stod(core::StringView view)
{
    return std::stod(view.data());
}
}

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

struct FixedStringHash
{
    template<size_t N>
    size_t operator()(const core::FixedString<N>& s) const noexcept
    {
        return core::StringUtility::Hash(s.view());
    }
};
}
