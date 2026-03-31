#pragma once

#include "core/TypeConcept.hpp"
#include "core/memory/Allocator.hpp"
#include "core/container/Iterator.hpp"
#include "core/debug/Exception.hpp"
#include <span>

namespace slug 
{
namespace core 
{
template<typename T>
using TSpan = std::span<T>;

#if 0
template<typename T>
class TSpan;

template<typename T, typename U>
concept InheritSpanType = std::derived_from<U, TSpan<T>>;

template<typename T>
class TSpan 
{
public:
    using iterator = TIterator<T>;
    using const_iterator = TIterator<T>;


    TSpan(T* data, size_t size)
        : m_data(data)
        , m_size(size)
    {}

    iterator begin() 
    {
        return iterator(m_data); 
    }

    iterator end() 
    { 
        return iterator(m_data + m_size);
    }

    const_iterator begin() const 
    { 
        return const_iterator(m_data); 
    }

    const_iterator end() const 
    { 
        return const_iterator(m_data + m_size); 
    }

    T* data() 
    {
        return m_data;
    }

    size_t size()
    {
        return m_size;
    }

    bool empty() 
    {
        return m_size == 0;
    }

    T* back() 
    {
        return m_data + m_size;
    }

    T at(size_t index) 
    {
        if (index < m_size) 
        {
            SLUG_THROW_EXCEPTION("Error. index is over size");
        }
        return *(m_data + index);
    }

    T& at(size_t index)
    {
        if (index < m_size)
        {
            SLUG_THROW_EXCEPTION("Error. index is over size");
        }
        return *(m_data + index);
    }

    const T& at(size_t index)
    {
        if (index < m_size)
        {
            SLUG_THROW_EXCEPTION("Error. index is over size");
        }
        return *(m_data + index);
    }

    template<InheritSpanType U>
    U operator=(const TSpan& span) 
    {
        U ret;
        ret = *this;
        return ret;
    }
protected:
    T* m_data;
    size_t m_size;
};
#endif
}
}
