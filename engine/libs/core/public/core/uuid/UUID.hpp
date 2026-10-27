
#pragma once

#include <cstddef>
#include <cstdint>

#include "core/Define.hpp"
#include "core/container/Array.hpp"
#include "core/container/String.hpp"

namespace slug::core
{
class UUID
{
public:
    using Bytes = TArray<uint8_t, 16>;

    constexpr UUID() noexcept = default;
    explicit constexpr UUID(const Bytes& bytes) noexcept
        : m_bytes(bytes)
    {
    }

    UUID(const UUID&) noexcept = default;
    UUID(UUID&&) noexcept = default;
    UUID& operator=(const UUID&) noexcept = default;
    UUID& operator=(UUID&&) noexcept = default;

    SLUG_NODISCARD static UUID Generate() noexcept;

    SLUG_NODISCARD bool operator==(const UUID& other) const noexcept
    {
        return m_bytes == other.m_bytes;
    }

    SLUG_NODISCARD bool IsValid() const noexcept;

    SLUG_NODISCARD size_t GetHash() const noexcept
    {
        return StringUtility::Hash(Get());
    }

    // バイナリ表現は RFC 9562 のバイト順で保持する。
    SLUG_NODISCARD StringView Get() const noexcept
    {
        return StringView(reinterpret_cast<const char*>(m_bytes.data()), m_bytes.size());
    }

    SLUG_NODISCARD const Bytes& GetBytes() const noexcept
    {
        return m_bytes;
    }

    void SetBytes(const Bytes& bytes) noexcept
    {
        m_bytes = bytes;
    }

    SLUG_NODISCARD bool TrySetBytes(StringView bytes) noexcept;
    SLUG_NODISCARD String ToString() const noexcept;
    SLUG_NODISCARD bool TrySetString(StringView text) noexcept;

private:
    Bytes m_bytes = {};
};

struct UUIDHash
{
    SLUG_NODISCARD size_t operator()(const UUID& uuid) const noexcept
    {
        return uuid.GetHash();
    }
};

}
