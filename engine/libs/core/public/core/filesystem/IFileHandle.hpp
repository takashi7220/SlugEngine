#pragma once

#include <cstdint>
#include "core/memory/ReferencePtr.hpp"

namespace slug::core
{

class IFileHandle;
using FileHandlePtr = core::TReferencePtr<IFileHandle>;

class IFileHandle : public core::ReferenceObject
{
public:
    virtual ~IFileHandle() = default;

    IFileHandle(const IFileHandle&) = delete;
    IFileHandle& operator=(const IFileHandle&) = delete;

    virtual bool IsValid() const = 0;
    virtual void Close() = 0;
    virtual uint64_t GetSize() const = 0;
    virtual bool ReadAt(uint64_t offset, void* dst, uint32_t size) = 0;

protected:
    IFileHandle() = default;
};
}
