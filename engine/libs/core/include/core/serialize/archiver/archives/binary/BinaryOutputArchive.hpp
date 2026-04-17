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

    virtual void BeginArray(size_t size) override
    {
        WriteBytes(size);
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

    void WriteBool(bool value)
    {
        WriteBytes(value);
    }

    template<IntegerType T>
    void WriteInteger(T value)
    {
        WriteBytes(value);
    }

    template<FloatType T>
    void WriteFloating(T value)
    {
        WriteBytes(value);
    }

    void WriteString(core::StringView value)
    {
        WriteInteger(value.size());
        if (!value.empty())
        {
            WriteBytes(value.data(), value.size());
        }
    }
private:
    template<typename T>
    void WriteBytes(const T& value)
    {
        const auto* p = reinterpret_cast<const uint8_t*>(&value);
        m_buffer.insert(m_buffer.end(), p, p + sizeof(T));
    }

    void WriteBytes(const void* data, size_t size)
    {
        const uint8_t* src = static_cast<const uint8_t*>(data);
        m_buffer.insert(m_buffer.end(), src, src + size);
    }

private:
    core::TVector<uint8_t>& m_buffer;
};
}
