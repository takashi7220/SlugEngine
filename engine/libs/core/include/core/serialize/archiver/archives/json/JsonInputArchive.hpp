#pragma once

#include "core/serialize/archiver/archives/InputArchive.hpp"
#include "core/memory/MemoryUtilities.hpp"
#include "core/debug/Exception.hpp"

namespace slug::core
{

class JsonInputArchive : public InputArchive<JsonInputArchive>
{
public:
    explicit JsonInputArchive(const core::TVector<uint8_t>& inBuffer)
        : m_buffer(inBuffer)
        , m_offset(0)
    {
    }

    virtual void BeginObject() override
    {
    }

    virtual void EndObject() override
    {
    }

    virtual void BeginField(core::StringView name) override
    {
    }

    virtual void EndField() override
    {
    }

    virtual size_t BeginArray() override
    {
        return ReadIntenger<size_t>();
    }

    virtual void EndArray() override
    {
    }

    virtual void BeginArrayElement() override
    {
    }

    virtual void EndArrayElement() override
    {
    }

    bool ReadBool()
    {
        bool value = false;
        ReadBytes(value);
        return value;
    }

    template<IntengerType T>
    T ReadIntenger()
    {
        T value = 0;
        ReadBytes(value);
        return value;
    }

    template<FloatType T>
    float ReadFloating()
    {
        T value = 0;
        ReadBytes(value);
        return value;
    }

    void ReadString(core::String& value)
    {
        size_t size = ReadIntenger<size_t>();
        value.resize(size);
        if (size > 0)
        {
            ReadBytes(value.data(), size);
        }
    }
private:
    template<typename T>
    void ReadBytes(T& value)
    {
        if (m_offset + sizeof(T) > m_buffer.size())
        {
            SLUG_THROW_EXCEPTION("BinaryInputArchive: buffer overrun.");
        }

        MemoryUtilities::Memcpy(&value, sizeof(T), m_buffer.data() + m_offset, sizeof(T));
        m_offset += sizeof(T);
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

private:
    const core::TVector<uint8_t>& m_buffer;
    std::size_t m_offset;

};

}
