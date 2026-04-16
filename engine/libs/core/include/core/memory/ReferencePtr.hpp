#pragma once

#include <cstdint>
#include <type_traits>
#include "core/Define.hpp"
#include "core/memory/MemoryObject.hpp"
#include "core/thread/Atomic.hpp"
#include "core/debug/Exception.hpp"

namespace slug::core
{

using ObjectType = uint32_t;
class ReferenceObject;

template<typename T>
concept ReferenceObjectType = std::derived_from<T, ReferenceObject>;

template<typename T>
concept HasAddRefAndRelease = requires(T t)
{
    {
        t.AddRef()
    } -> std::same_as<unsigned long>;
    {
        t.Release()
    } -> std::same_as<unsigned long>;
};

struct InternalObject
{
    union
    {
        uint64_t integer;
        void* pointer;
    };

    explicit InternalObject(uint64_t i)
        : integer(i)
    {
    };

    InternalObject(void* p)
        : pointer(p)
    {
    };

    template<typename T>
    operator T* () const
    {
        return static_cast<T*>(pointer);
    }
};

struct ControlBlock : public MemoryObject
{
    TAtomic<unsigned long> strong { 0 };
    TAtomic<unsigned long> weak { 1 };
    TAtomic<bool> alive { true };
    TAtomic<void*> object_ptr { nullptr };

    explicit ControlBlock(void* obj) : object_ptr(obj)
    {}

    void AddWeak() noexcept
    {
        weak.fetch_add(1, std::memory_order_relaxed);
    }

    unsigned long ReleaseWeak() noexcept
    {
        if (weak.fetch_sub(1, std::memory_order_acq_rel) == 1)
        {
            delete this;
            return 0;
        }
        return weak.load(std::memory_order_relaxed);
    }

    bool TryIncStrongFromWeak() noexcept
    {
        if (!alive.load(std::memory_order_acquire))
        {
            return false;
        }

        unsigned long s = strong.load(std::memory_order_acquire);
        while (s != 0)
        {
            if (strong.compare_exchange_weak(
                s, s + 1,
                std::memory_order_acq_rel,
                std::memory_order_acquire))
            {
                return true;
            }
        }
        return false;
    }

    void* GetObjectPtrAcquire() const noexcept
    {
        return object_ptr.load(std::memory_order_acquire);
    }

    void ClearObjectPtr() noexcept
    {
        object_ptr.store(nullptr, std::memory_order_release);
    }
};

class ReferenceObject : public MemoryObject
{
public:
    ReferenceObject(const ReferenceObject&) = delete;
    ReferenceObject(ReferenceObject&&) = delete;
    ReferenceObject& operator=(const ReferenceObject&) = delete;
    ReferenceObject& operator=(ReferenceObject&&) = delete;

    virtual unsigned long AddRef() noexcept
    {
        return m_control->strong.fetch_add(1, std::memory_order_relaxed) + 1;
    }

    virtual unsigned long AddWeakRef() noexcept
    {
        m_control->AddWeak();
        return m_control->weak.load(std::memory_order_relaxed);
    }

    virtual unsigned long Release() noexcept
    {
        if (m_control->strong.fetch_sub(1, std::memory_order_acq_rel) == 1)
        {
            ControlBlock* cb = m_control;

            cb->alive.store(false, std::memory_order_release);
            cb->ClearObjectPtr();

            delete this;

            cb->ReleaseWeak();
            return 0;
        }
        return m_control->strong.load(std::memory_order_relaxed);
    }

    virtual unsigned long ReleaseWeak() noexcept
    {
        return m_control->ReleaseWeak();
    }

    bool TryAddRefFromWeak() noexcept
    {
        return m_control->TryIncStrongFromWeak();
    }

    virtual InternalObject GetNativeObject(ObjectType object_type)
    {
        (void)object_type;
        return nullptr;
    }

    unsigned long GetReferenceCount() const noexcept
    {
        return m_control->strong.load(std::memory_order_relaxed);
    }

    ControlBlock* Control() noexcept
    {
        return m_control;
    }

protected:
    ReferenceObject()
        : m_control(new ControlBlock(this))
    {
        ControlBlock* cb = m_control;
        if (cb)
        {
            cb->strong.store(0, std::memory_order_relaxed);
        }
    }

    virtual ~ReferenceObject() override
    {
        ControlBlock* cb = m_control;
        if (!cb)
        {
            return;
        }

        cb->alive.store(false, std::memory_order_release);
        cb->ClearObjectPtr();
        cb->ReleaseWeak();

        m_control = nullptr;
    }

private:
    ControlBlock* m_control;
};

struct AdoptRefT
{
    explicit AdoptRefT() = default;
};
inline constexpr AdoptRefT adoptRef {};

template <typename T>
class TReferencePtr
{
public:
    using ObjectType = T;

    TReferencePtr() noexcept = default;
    TReferencePtr(std::nullptr_t) noexcept : m_ptr(nullptr)
    {}

    TReferencePtr(T* other) noexcept
        : m_ptr(other)
    {
        InternalAddRef();
    }

    TReferencePtr(T* other, AdoptRefT) noexcept
        : m_ptr(other)
    {}

    TReferencePtr(const TReferencePtr& other) noexcept
        : m_ptr(other.m_ptr)
    {
        InternalAddRef();
    }

    TReferencePtr(TReferencePtr&& other) noexcept
        : m_ptr(other.m_ptr)
    {
        other.m_ptr = nullptr;
    }

    template<class U>
    TReferencePtr(TReferencePtr<U>&& other,
        typename std::enable_if<std::is_convertible<U*, T*>::value, void*>::type* = nullptr) noexcept
        : m_ptr(other.get())
    {
        other.detach();
    }

    ~TReferencePtr() noexcept
    {
        InternalRelease();
    }

    TReferencePtr& operator=(std::nullptr_t) noexcept
    {
        InternalRelease();
        m_ptr = nullptr;
        return *this;
    }

    TReferencePtr& operator=(T* other) noexcept
    {
        TReferencePtr tmp(other);
        tmp.swap(*this);
        return *this;
    }

    TReferencePtr& operator=(const TReferencePtr& other) noexcept
    {
        TReferencePtr tmp(other);
        tmp.swap(*this);
        return *this;
    }

    TReferencePtr& operator=(TReferencePtr&& other) noexcept
    {
        if (this->m_ptr != other.m_ptr)
        {
            InternalRelease();
            m_ptr = other.m_ptr;
            other.m_ptr = nullptr;
        }
        return *this;
    }

    static TReferencePtr create(T* other) noexcept
    {
        return TReferencePtr(other);
    }

    void swap(TReferencePtr& r) noexcept
    {
        T* tmp = m_ptr;
        m_ptr = r.m_ptr;
        r.m_ptr = tmp;
    }

    SLUG_NODISCARD T* get() const noexcept
    {
        return m_ptr;
    }

    SLUG_NODISCARD T** operator&() noexcept
    {
        return get_address_of();
    }

    SLUG_NODISCARD T* const* operator&() const noexcept
    {
        return get_address_of();
    }

    operator T* () const noexcept
    {
        return m_ptr;
    }

    T* operator->() const noexcept
    {
        return m_ptr;
    }

    SLUG_NODISCARD T* const* get_address_of() const noexcept
    {
        return &m_ptr;
    }

    SLUG_NODISCARD T** get_address_of() noexcept
    {
        return &m_ptr;
    }

    SLUG_NODISCARD T** release_and_get_address_of() noexcept
    {
        InternalRelease();
        m_ptr = nullptr;
        return &m_ptr;
    }

    T* detach() noexcept
    {
        T* ptr = m_ptr;
        m_ptr = nullptr;
        return ptr;
    }

    void attach(T* other) noexcept
    {
        if (m_ptr == other) return;

        InternalRelease();
        m_ptr = other; // attach は所有を "採用"（AddRef しない）
    }

    void reset() noexcept
    {
        InternalRelease();
        m_ptr = nullptr;
    }

    void reset(T* other) noexcept
    {
        if (other) other->AddRef();

        T* old = std::exchange(m_ptr, other);
        if (old) old->Release();
    }

private:
    void InternalAddRef() const noexcept
    {
        if (m_ptr) m_ptr->AddRef();
    }

    void InternalRelease() noexcept
    {
        T* temp = m_ptr;
        if (temp)
        {
            m_ptr = nullptr;
            temp->Release();
        }
    }

private:
    T* m_ptr = nullptr;
};

template<HasAddRefAndRelease T, class... Args>
TReferencePtr<T> MakeReference(Args&&... args)
{
    return TReferencePtr<T>(new T(std::forward<Args>(args)...));
}

}
