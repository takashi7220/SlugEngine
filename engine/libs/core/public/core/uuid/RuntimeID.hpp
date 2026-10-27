#pragma once

#include <cstddef>
#include <cstdint>

#include "core/Define.hpp"

namespace slug::core
{

class RuntimeID
{
public:
    constexpr RuntimeID() noexcept = default;
    RuntimeID(const RuntimeID&) noexcept = default;
    RuntimeID(RuntimeID&&) noexcept = default;
    RuntimeID& operator=(const RuntimeID&) noexcept = default;
    RuntimeID& operator=(RuntimeID&&) noexcept = default;

    SLUG_NODISCARD static RuntimeID Generate() noexcept;
    SLUG_NODISCARD static bool TryGenerate(RuntimeID& outID) noexcept;

    SLUG_NODISCARD constexpr bool operator==(const RuntimeID&) const noexcept = default;

    SLUG_NODISCARD constexpr bool IsValid() const noexcept
    {
        return m_value != 0;
    }

    SLUG_NODISCARD constexpr uint64_t GetValue() const noexcept
    {
        return m_value;
    }

    SLUG_NODISCARD constexpr size_t GetHash() const noexcept
    {
        if constexpr (sizeof(size_t) < sizeof(uint64_t))
        {
            return static_cast<size_t>(m_value ^ (m_value >> 32));
        }
        return static_cast<size_t>(m_value);
    }

private:
    explicit constexpr RuntimeID(uint64_t value) noexcept
        : m_value(value)
    {
    }

    uint64_t m_value = 0;
};

struct RuntimeIDHash
{
    SLUG_NODISCARD size_t operator()(const RuntimeID& id) const noexcept
    {
        return id.GetHash();
    }
};

}
