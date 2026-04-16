#pragma once

#include "core/memory/ReferencePtr.hpp"

namespace slug::core
{
template <typename T>
class TWeakReferencePtr
{
public:
    TWeakReferencePtr() noexcept = default;

    TWeakReferencePtr(std::nullptr_t) noexcept : m_cb(nullptr)
    {}

    explicit TWeakReferencePtr(T* raw) noexcept
        : m_cb(raw ? raw->Control() : nullptr)
    {
        if (m_cb) m_cb->AddWeak();
    }

    TWeakReferencePtr(const TReferencePtr<T>& strong) noexcept
        : m_cb(strong ? strong->Control() : nullptr)
    {
        if (m_cb) m_cb->AddWeak();
    }

    TWeakReferencePtr(const TWeakReferencePtr& other) noexcept
        : m_cb(other.m_cb)
    {
        if (m_cb) m_cb->AddWeak();
    }

    TWeakReferencePtr(TWeakReferencePtr&& other) noexcept
        : m_cb(other.m_cb)
    {
        other.m_cb = nullptr;
    }

    ~TWeakReferencePtr() noexcept
    {
        if (m_cb) m_cb->ReleaseWeak();
    }

    TWeakReferencePtr& operator=(std::nullptr_t) noexcept
    {
        reset();
        return *this;
    }

    TWeakReferencePtr& operator=(const TWeakReferencePtr& other) noexcept
    {
        if (this == &other) return *this;
        reset();
        m_cb = other.m_cb;
        if (m_cb) m_cb->AddWeak();
        return *this;
    }

    TWeakReferencePtr& operator=(TWeakReferencePtr&& other) noexcept
    {
        if (this == &other) return *this;
        reset();
        m_cb = other.m_cb;
        other.m_cb = nullptr;
        return *this;
    }

    void reset() noexcept
    {
        if (m_cb)
        {
            m_cb->ReleaseWeak();
            m_cb = nullptr;
        }
    }

    bool expired() const noexcept
    {
        if (!m_cb) return true;
        return !m_cb->alive.load(std::memory_order_acquire) ||
            (m_cb->strong.load(std::memory_order_acquire) == 0);
    }

    TReferencePtr<T> lock() const noexcept
    {
        if (!m_cb) return TReferencePtr<T>(nullptr);

        if (!m_cb->TryIncStrongFromWeak())
        {
            return TReferencePtr<T>(nullptr);
        }

        void* p = m_cb->GetObjectPtrAcquire();
        if (!p)
        {
            m_cb->strong.fetch_sub(1, std::memory_order_acq_rel);
            return TReferencePtr<T>(nullptr);
        }

        return TReferencePtr<T>(static_cast<T*>(p), adoptRef);
    }

    T* get_unsafe() const noexcept
    {
        if (!m_cb)
        {
            return nullptr;
        }
        return static_cast<T*>(m_cb->GetObjectPtrAcquire());
    }

    void swap(TWeakReferencePtr& other) noexcept
    {
        ControlBlock* tmp = m_cb;
        m_cb = other.m_cb;
        other.m_cb = tmp;
    }

private:
    ControlBlock* m_cb = nullptr;
};
}
