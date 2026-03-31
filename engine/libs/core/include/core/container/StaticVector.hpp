#pragma once

#include "core/container/Array.hpp"
#include "core/debug/Assert.hpp"
namespace slug 
{
namespace core
{

template<typename T, size_t Size>
class TStaticVector : public TArray<T, Size> 
{
public:
    static constexpr size_t MAX_ELEMENT = Size;

    using base = TArray<T, Size>;
    using value_type =  base::value_type;
    using size_type =  base::size_type;
    using difference_type = base::difference_type;
    using reference = base::reference;
    using const_reference = base::const_reference;
    using iterator =  base::iterator;
    using const_iterator = base::const_iterator;

    TStaticVector()
        : base()
    {}

    TStaticVector(size_t size)
        : base()
        , m_current_size(size)
    {
        SLUG_ASSERT(size <= MAX_ELEMENT);
    }

    TStaticVector(const TStaticVector& other) = default;

    TStaticVector(std::initializer_list<T> il)
    {
        for (auto i : il)
        {
            push_back(i);
        }
    }

    reference operator[] (size_type pos)
    {
        SLUG_ASSERT(pos < m_current_size);
        return base::operator[](pos);
    }

    const_reference operator[] (size_type pos) const
    {
        SLUG_ASSERT(pos < m_current_size);
        return base::operator[](pos);
    }

    using base::front;

    reference back() noexcept
    {
        auto tmp = end(); --tmp; return *tmp;
    }

    const_reference back() const noexcept
    {
        auto tmp = cend(); --tmp; return *tmp;
    }

    iterator end() noexcept
    {
        return iterator(base::begin()) + m_current_size;
    }

    const_iterator end() const noexcept
    {
        return base::cend();
    }

    const_iterator cend() const noexcept
    {
        return const_iterator(base::cbegin()) + m_current_size;
    }

    bool empty() const noexcept
    {
        return m_current_size == 0;
    }

    size_t size() const noexcept
    {
        return m_current_size;
    }

    constexpr size_t max_size() const noexcept
    {
        return MAX_ELEMENT;
    }

    void fill(const T& value) noexcept
    {
        base::fill(value);
        m_current_size = MAX_ELEMENT;
    }

    void swap(TStaticVector& other) noexcept
    {
        base::swap(*this);
        std::swap(m_current_size, other.m_current_size);
    }

    void push_back(const T& value) noexcept
    {
        SLUG_ASSERT(m_current_size < MAX_ELEMENT);
        *(base::data() + m_current_size) = value;
        m_current_size++;
    }

    void push_back(T&& value) noexcept
    {
        SLUG_ASSERT(m_current_size < MAX_ELEMENT);
        *(base::data() + m_current_size) = std::move(value);
        m_current_size++;
    }

    void pop_back() noexcept
    {
        SLUG_ASSERT(m_current_size > 0);
        m_current_size--;
    }

    void resize(size_type new_size) noexcept
    {
        SLUG_ASSERT(new_size <= MAX_ELEMENT);

        if (m_current_size > new_size) {
            for (size_type i = new_size; i < m_current_size; i++)
            {
                (base::data() + i)->~T();
            }
        }
        else 
        {
            for (size_type i = m_current_size; i < new_size; i++) 
            {
                new(base::data() + i) T();
            }
        }

        m_current_size = new_size;
    }

    reference emplace_back() noexcept
    {
        resize(m_current_size + 1);
        return back();
    }

    
private:
    size_t m_current_size = 0;
};

}
}
