#pragma once

#include <cstdint>

namespace slug
{
namespace core
{
template<typename T>
class TSingleton 
{
public:
    static T& Get()
    {
        if (!m_ptr) 
        {
            Create();
        }
        return *m_ptr;
    }

    static void Create()
    {
        if (!m_ptr) 
        {
            m_ptr = new T;
        }
    }

    static void Destroy()
    {
        if (m_ptr) 
        {
            delete(m_ptr);
            m_ptr = nullptr;
        }
    }

protected:
    TSingleton() = default;
    virtual~TSingleton() = default;

private:
    static T* m_ptr;
};

template <typename T> 
T* TSingleton<T>::m_ptr = nullptr;
}
}
