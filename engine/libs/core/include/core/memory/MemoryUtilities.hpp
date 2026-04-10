#pragma once

#include <cstdio>
#include "core/memory/MemoryLabel.hpp"
#include "core/debug/Exception.hpp"

#if defined(DEBUG_MODE)
#include "core/memory/debug/MemoryDebugUtilities.hpp"
#endif

#define SLUG_SAFE_DELETE(v)            if( v ){ delete ( v ); v = nullptr;     }
#define SLUG_SAFE_DELETE_ARRAY(v)    if( v ){ delete [] ( v ); v = nullptr;    }
#define SLUG_SAFE_RELEASE(v)         if( v ){ ( v )->Release(); }
#define SLUG_SAFE_DESTROY(v)         if( v ){ ( v )->destroy(); v = nullptr; }

#if defined(DEBUG_MODE)
#define SLUG_MEMORY_LEACK_CHECK_SCOPE(label) slug::core::MemoryLeakCheckScope _memory_leak_check_(label);
#define SLUG_MEMORY_LABEL_REGISTER(label, name) 
#else
#define SLUG_MEMORY_LEACK_CHECK_SCOPE(label)
#define SLUG_MEMORY_LABEL_REGISTER(label, name) 
#endif

namespace slug::core
{

enum MemoryLabelType : MemoryLabelId
{
    MemoryLabelType_System = 0,
    MemoryLabelType_Debug,
    MemoryLabelType_Graphics,
    MemoryLabelType_Physix,
    MemoryLabelType_Sound,
    MemoryLabelType_Unknown = 29,
    MemoryLabelType_SystemMax = 30,
};

struct MemoryHeader
{
    size_t size;
    MemoryLabel label;
};

class MemoryUtilities
{
public:
    static void* Allocate(size_t size);
    static void* Allocate(size_t size, size_t align);
    static void Deallocate(void* p);
    static void Deallocate(void* p, size_t align);

    static void Memset(void* dst, int32_t value, size_t dstSize);
    static void Memcpy(void* dst, size_t dstSize, const void* src, size_t srcSize);

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
};

}
