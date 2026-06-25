#pragma once

#include "core/container/Vector.hpp"
#include "core/container/String.hpp"
#include "core/memory/ReferencePtr.hpp"

namespace slug::core
{

class FileBlob;
using FileBlobPtr = TReferencePtr<FileBlob>;

class FileBlob : public ReferenceObject
{
public:
    explicit FileBlob()
        : m_data()
    {
    }

    explicit FileBlob(const core::TVector<uint8_t>& data)
        : m_data(data)
    {
    }

    virtual ~FileBlob()
    {
    }

    core::TVector<uint8_t>& GetData()
    {
        return m_data;
    }

    const core::TVector<uint8_t>& GetData() const
    {
        return m_data;
    }

private:
    core::TVector<uint8_t> m_data;
};
}
