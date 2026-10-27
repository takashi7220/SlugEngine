#include "core/uuid/UUID.hpp"

#include <cstdlib>
#include <Windows.h>
#include <bcrypt.h>

#include "core/debug/Assert.hpp"

namespace slug::core
{
namespace
{

int ParseHexDigit(char value) noexcept
{
    if (value >= '0' && value <= '9')
    {
        return value - '0';
    }
    if (value >= 'a' && value <= 'f')
    {
        return value - 'a' + 10;
    }
    if (value >= 'A' && value <= 'F')
    {
        return value - 'A' + 10;
    }
    return -1;
}

}

UUID UUID::Generate() noexcept
{
    Bytes bytes = {};
    const NTSTATUS status = BCryptGenRandom(nullptr, bytes.data(), static_cast<ULONG>(bytes.size()),
        BCRYPT_USE_SYSTEM_PREFERRED_RNG);
    if (status < 0)
    {
        SLUG_ASSERT_MSG(false, "Failed to generate UUID random bytes");
        std::abort();
    }

    bytes[6] = static_cast<uint8_t>((bytes[6] & 0x0Fu) | 0x40u);
    bytes[8] = static_cast<uint8_t>((bytes[8] & 0x3Fu) | 0x80u);
    return UUID(bytes);
}

bool UUID::IsValid() const noexcept
{
    for (uint8_t byte : m_bytes)
    {
        if (byte != 0)
        {
            return true;
        }
    }
    return false;
}

bool UUID::TrySetBytes(StringView bytes) noexcept
{
    if (bytes.size() != m_bytes.size())
    {
        return false;
    }
    Bytes parsed = {};
    for (size_t i = 0; i < parsed.size(); ++i)
    {
        parsed[i] = static_cast<uint8_t>(bytes[i]);
    }
    m_bytes = parsed;
    return true;
}

String UUID::ToString() const noexcept
{
    constexpr char HexDigits[] = "0123456789abcdef";
    char text[36];
    size_t position = 0;
    for (size_t i = 0; i < m_bytes.size(); ++i)
    {
        if (i == 4 || i == 6 || i == 8 || i == 10)
        {
            text[position++] = '-';
        }
        text[position++] = HexDigits[m_bytes[i] >> 4];
        text[position++] = HexDigits[m_bytes[i] & 0x0F];
    }
    return String(text, sizeof(text));
}

bool UUID::TrySetString(StringView text) noexcept
{
    if (text.size() != 36)
    {
        return false;
    }
    Bytes parsed = {};
    size_t position = 0;
    for (size_t i = 0; i < parsed.size(); ++i)
    {
        if (i == 4 || i == 6 || i == 8 || i == 10)
        {
            if (text[position++] != '-')
            {
                return false;
            }
        }
        const int high = ParseHexDigit(text[position++]);
        const int low = ParseHexDigit(text[position++]);
        if (high < 0 || low < 0)
        {
            return false;
        }
        parsed[i] = static_cast<uint8_t>((high << 4) | low);
    }
    m_bytes = parsed;
    return true;
}
}
