#pragma once

#include "core/serialize/archiver/archives/InputArchive.hpp"
#include "core/memory/MemoryUtilities.hpp"
#include "core/debug/Exception.hpp"

namespace slug::core
{

class JsonInputArchive : public InputArchive<JsonInputArchive>
{
public:
    explicit JsonInputArchive(const core::String& buffer)
        : m_buffer(buffer)
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
    }

    template<IntegerType T>
    T ReadInteger()
    {
    }

    template<FloatType T>
    float ReadFloating()
    {
    }

    void ReadString(core::String& value)
    {
    }
private:

private:
    const core::String& m_buffer;
};

}
