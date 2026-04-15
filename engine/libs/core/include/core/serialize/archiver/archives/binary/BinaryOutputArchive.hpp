#pragma once

#include "core/serialize/archiver/archives/OutputArchive.hpp"

namespace slug::core
{
class BinaryOutputArchive : public OutputArchive<BinaryOutputArchive>
{
public:
    explicit BinaryOutputArchive(core::TVector<uint8_t>& outBuffer)
        : m_buffer(outBuffer)
    {
    }

    void WriteBytes(const void* data, size_t size)
    {
        const uint8_t* src = static_cast<const uint8_t*>(data);
        m_buffer.insert(m_buffer.end(), src, src + size);
    }

    virtual void BeginNamedValue(core::StringView name) override
    {
    }

    virtual void EndNamedValue() override
    {
    }
private:
    core::TVector<uint8_t>& m_buffer;
};
}
