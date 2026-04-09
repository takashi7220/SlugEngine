#pragma once

#include <cstdio>
#include "core/debug/Exception.hpp"
#include "core/memory/MemoryCounter.hpp"

#define SLUG_SAFE_DELETE(v)            if( v ){ delete ( v ); v = nullptr;     }
#define SLUG_SAFE_DELETE_ARRAY(v)    if( v ){ delete [] ( v ); v = nullptr;    }
#define SLUG_SAFE_RELEASE(v)         if( v ){ ( v )->Release(); }
#define SLUG_SAFE_DESTROY(v)         if( v ){ ( v )->destroy(); v = nullptr; }

#if defined(DEBUG_MODE)
#define SLUG_MEMORY_LEACK_CHECK_SCOPE(label) slug::core::MemoryLeakCheckScope _memory_leak_check_##label(#label);
#define SLUG_MEMORY_SYSTEM_LABEL "System"
#else
#define SLUG_MEMORY_LEACK_CHECK_SCOPE(label)
#define SLUG_MEMORY_SYSTEM_LABEL
#endif

namespace slug::core
{
struct MemoryProfileInfo
{
    size_t currentUseMemorySize = 0;
    size_t availableUseMemorySize = 0;
    size_t currentCommitMemorySize = 0;
    size_t peakCommitMemorySize = 0;

    bool operator==(const MemoryProfileInfo& v)
    {
        bool same = true;
        same &= (this->currentUseMemorySize == v.currentUseMemorySize);
        same &= (this->availableUseMemorySize == v.availableUseMemorySize);
        same &= (this->currentCommitMemorySize == v.currentUseMemorySize);
        same &= (this->peakCommitMemorySize == v.peakCommitMemorySize);
        return same;
    }
};

using MemoryLeakCheckCallback = void(*)(size_t leakMemorySize, const char* label);

class MemoryLeakCheckScope final
{
public:
    MemoryLeakCheckScope(const char* label);
    ~MemoryLeakCheckScope();

private:
    MemoryLeakCheckCallback m_callback;
    size_t m_checkSize;
    char m_label[SLUG_MEMORY_LABLE_STR_LENGTH];
};

class MemoryUtilities
{
public:
    static void* Allocate(size_t size, const char* label = SLUG_MEMORY_SYSTEM_LABEL);
    static void* Allocate(size_t size, size_t align, const char* label = SLUG_MEMORY_SYSTEM_LABEL);
    static void Deallocate(void* p, const char* label = SLUG_MEMORY_SYSTEM_LABEL);
    static void Deallocate(void* p, size_t align, const char* label = SLUG_MEMORY_SYSTEM_LABEL);

    static void Memset(void* dst, int32_t value, size_t dstSize);
    static void Memcpy(void* dst, size_t dstSize, const void* src, size_t srcSize);

    static MemoryProfileInfo GetMemoryProfileInfo();
    static void SetMemoryCounter(MemoryCounter* counter);
    static void SetMemoryLeackCheckCallback(MemoryLeakCheckCallback callback);
    static size_t GetCurrentMemorySize(const char* label);

public:
    template<typename T, typename U>
    static T CheckedCast(U u)
    {
        static_assert(!(std::is_same<T, U>::value == true), "Redundant checked_cast");

#if defined(DEBUG_MODE)
        if (!u)
        {
            return nullptr;
        }

        T t = dynamic_cast<T>(u);
        if (!t)
        {
            SLUG_THROW_EXCEPTION("Invalid type cast");
        }
        return t;
#else
        return static_cast<T>(u);
#endif
    }

private:
    static void RecordAllocate(void* p, const char* label);
    static void RecordDeallocate(void* p, const char* label);
};

}
