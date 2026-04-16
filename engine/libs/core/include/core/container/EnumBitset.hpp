#pragma once

#include "core/container/Bitset.hpp"
#include "core/container/String.hpp"
#include "core/TypeConcept.hpp"

namespace slug::core
{

class EnumBitSetUtility
{
public:
    static bool Check(uint32_t value, uint32_t flag)
    {
        return (value & flag) != 0;
    }

    static bool Equal(uint32_t value, uint32_t flag)
    {
        return value == flag;
    }

    static bool Set(uint32_t& value, uint32_t flag)
    {
        value |= flag;
    }

    static bool Clear(uint32_t& value, uint32_t flag)
    {
        value &= ~flag;
    }

    static void Toggle(uint32_t& value, uint32_t flag)
    {
        value ^= flag;
    }
};

template<typename T>
concept EnumWithCount = std::is_enum_v<T> && requires 
{
    T::Count;
};

template<EnumWithCount T>
class TEnumBitset 
{
public:
    using BitsetType = TBitset<static_cast<size_t>(T::Count)>;

    TEnumBitset() 
        : m_bits() 
    {}

    TEnumBitset(BitsetType bits)
    {
        m_bits = bits;
    }

    TEnumBitset(T bitFlag)
    {
        m_bits.set(static_cast<size_t>(bitFlag));
    }

    void operator=(T bitFlag)
    {
        m_bits.set(static_cast<size_t>(bitFlag));
    }

    void Set(T pos) 
    {
        m_bits.set(static_cast<size_t>(pos));
    }

    void Set() 
    {
        m_bits.set();
    }

    void Reset(T pos) 
    {
        m_bits.reset(static_cast<size_t>(pos));
    }

    void Reset()
    {
        m_bits.reset();
    }

    bool Test(T pos) const 
    {
        return m_bits.test(static_cast<size_t>(pos));
    }

    void All() 
    {
        m_bits.all();
    }

    bool Any() 
    {
        return m_bits.any();
    }

    BitsetType GetBitset() const 
    {
        return m_bits;
    }

    core::StringView ToString() const 
    {
        return m_bits.to_string().c_str();
    }

    constexpr bool operator==(const TEnumBitset& v0) const
    {
        return v0.GetBitset() == m_bits;
    }

    constexpr bool operator!=(const TEnumBitset& v0) const
    {
        return v0.GetBitset() != m_bits;
    }

    constexpr TEnumBitset operator&(const TEnumBitset& v0) const
    {
        return m_bits & v0.GetBitset();
    }

    constexpr TEnumBitset operator | (const TEnumBitset& v0) const
    {
        return m_bits | v0.GetBitset();
    }

private:
    BitsetType m_bits;
};
}
