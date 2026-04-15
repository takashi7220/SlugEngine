#pragma once

#include "core/serialize/archiver/archives/InputArchive.hpp"
#include "core/memory/MemoryUtilities.hpp"
#include "core/debug/Exception.hpp"

namespace slug::core
{

class BinaryInputArchive : public InputArchive<BinaryInputArchive>
{
public:
    explicit BinaryInputArchive(const core::TVector<uint8_t>& inBuffer)
        : m_buffer(inBuffer)
        , m_offset(0)
    {
    }

    void ReadBytes(void* dst, std::size_t size)
    {
        if (m_offset + size > m_buffer.size())
        {
            SLUG_THROW_EXCEPTION("BinaryInputArchive: buffer overrun.");
        }

        MemoryUtilities::Memcpy(dst, size, m_buffer.data() + m_offset, size);
        m_offset += size;
    }

    virtual void BeginNamedValue(core::StringView name) override
    {
    }

    virtual void EndNamedValue() override
    {
    }

private:
    const core::TVector<uint8_t>& m_buffer;
    std::size_t m_offset;

};

}
