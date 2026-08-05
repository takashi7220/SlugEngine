#pragma once

#include <cstdio>
#include "core/Define.hpp"
#include "core/memory/MemoryLabel.hpp"
#include "core/debug/Exception.hpp"
#include "core/memory/debug/MemoryDebugUtility.hpp"

#define SLUG_SAFE_DELETE(v)            if( v ){ delete ( v ); v = nullptr;     }
#define SLUG_SAFE_DELETE_ARRAY(v)    if( v ){ delete [] ( v ); v = nullptr;    }
#define SLUG_SAFE_RELEASE(v)         if( v ){ ( v )->Release(); }
#define SLUG_SAFE_DESTROY(v)         if( v ){ ( v )->destroy(); v = nullptr; }

#if SLUG_ENABLE_MEMORY_COUNTER
#define SLUG_MEMORY_LABEL_SCOPE(label) slug::core::MemoryLabelScope _memory_label_scope_##label(#label);
#define SLUG_MEMORY_SUBLABEL_SCOPE(label) slug::core::MemorySubLabelScope _memory_sublabel_scope_##label(#label);
#define SLUG_MEMORY_LEACK_CHECK_SCOPE(label, subLabel) slug::core::MemoryLeakCheckScope _memory_leak_check_##label##_##subLabel(#label, #subLabel);
#else
#define SLUG_MEMORY_LABEL_SCOPE(label) 
#define SLUG_MEMORY_SUBLABEL_SCOPE(label) 
#define SLUG_MEMORY_LEACK_CHECK_SCOPE(label, subLabel)
#define SLUG_MEMORY_LABEL_REGISTER(label, name) 
#endif

namespace slug::core
{

struct MemoryProfile
{
    size_t malloc_requested_current = 0;
    size_t malloc_requested_peak = 0;
    size_t commited_current = 0;
    size_t reserved_current = 0;
};

namespace MemoryUtility
{
    void* Allocate(size_t size);
    void* Allocate(size_t size, size_t align);
    void Deallocate(void* p);
    void Deallocate(void* p, size_t align);

    void Memset(void* dst, int32_t value, size_t dstSize);
    void Memcpy(void* dst, size_t dstSize, const void* src, size_t srcSize);
    MemoryProfile GetCurrentProfile();
    void EnableDebugLayer();
}
}
