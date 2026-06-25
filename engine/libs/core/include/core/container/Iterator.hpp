#pragma once

namespace slug::core
{
template<typename T>
class TIterator
{
public:
    explicit TIterator(T* ptr) 
        : m_ptr(ptr) 
    {}

    TIterator& operator++() 
    {
        ++m_ptr;
        return *this;
    }

    bool operator==(const TIterator& other) const 
    {
        return m_ptr == other.m_ptr;
    }

    bool operator!=(const TIterator& other) const 
    {
        return m_ptr != other.m_ptr;
    }

    T& operator*() 
    {
        return *m_ptr;
    }

private:
    T* m_ptr;
};
}