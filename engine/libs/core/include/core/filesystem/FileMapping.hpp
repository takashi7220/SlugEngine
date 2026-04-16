#pragma once

#include "core/memory/ReferencePtr.hpp"
#include "core/memory/UniquePtr.hpp"
#include "core/container/String.hpp"

namespace slug::core
{

class FileMappingObject : public ReferenceObject
{
public:
    static FileMappingObject* Create(core::StringView filePath);

    FileMappingObject(core::StringView filePath);
    virtual ~FileMappingObject();
    void Open(core::StringView filePath);
    void Close();
    void* GetData();
    size_t GetSize();

private:
    struct Implement;
    core::TUniquePtr<Implement> m_impl = nullptr;
};
using FileMappingObjectPtr = core::TReferencePtr<FileMappingObject>;

}
