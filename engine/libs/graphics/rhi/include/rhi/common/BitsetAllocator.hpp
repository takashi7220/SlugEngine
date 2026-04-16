#pragma once

#include "core/container/Vector.hpp"
#include "core/thread/Mutex.hpp"

namespace slug::rhi
{
class BitsetAllocator 
{
public:
    explicit BitsetAllocator(size_t capacity, bool multiThreaded) 
        : m_multiThreaded(multiThreaded)
    {
        m_allocated.resize(capacity);
    }

    int32_t Allocate() 
    {
        if (m_multiThreaded) 
        {
            m_mutex.lock();
        }

        int32_t result = -1;

        int32_t capacity = static_cast<int32_t>(m_allocated.size());
        for (int32_t i = 0; i < capacity; i++) {
            int32_t ii = (m_nextAvailable + i) % capacity;

            if (!m_allocated[ii]) 
            {
                result = ii;
                m_nextAvailable = (ii + 1) % capacity;
                m_allocated[ii] = true;
                break;
            }
        }

        if (m_multiThreaded) 
        {
            m_mutex.unlock();
        }

        return result;
    }

    void Release(int32_t index) 
    {
        if (index >= 0 && index < static_cast<int32_t>(m_allocated.size())) 
        {
            if (m_multiThreaded) 
            {
                m_mutex.lock();
            }

            m_allocated[index] = false;
            m_nextAvailable = (m_nextAvailable, index);

            if (m_multiThreaded) 
            {
                m_mutex.unlock();
            }
        }
    }

    size_t GetCapacity() const 
    {
        return m_allocated.size();
    }
private:
    int32_t m_nextAvailable = 0;
    core::TVector<bool> m_allocated = {};
    bool m_multiThreaded = true;
    core::Mutex m_mutex;
};

}