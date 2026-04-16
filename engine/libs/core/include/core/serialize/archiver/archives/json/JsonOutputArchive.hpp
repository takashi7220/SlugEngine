#pragma once

#include "core/serialize/archiver/archives/OutputArchive.hpp"

namespace slug::core
{
class BinaryOutputArchive : public OutputArchive<BinaryOutputArchive>
{
public:
    explicit BinaryOutputArchive(core::String& outBuffer)
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
    }

    template<IntengerType T>
    void WriteIntenger(T value)
    {
    }

    template<FloatType T>
    void WriteFloating(T value)
    {
    }

    void WriteString(core::StringView value)
    {
    }
private:

private:
    core::String& m_buffer;
};
}
