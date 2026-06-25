#pragma once

#include "core/serialize/archiver/archives/InputArchive.hpp"
#include "core/serialize/archiver/archives/json/JsonParser.hpp"
#include "core/debug/Exception.hpp"
#include "core/container/Vector.hpp"

#include <type_traits>

namespace slug::core
{

class JsonInputArchive : public InputArchive<JsonInputArchive>
{
    struct Frame
    {
        const JsonValue* node = nullptr;
        size_t arrayIndex = 0;
    };

public:
    explicit JsonInputArchive(const core::String& buffer)
        : m_root(ParseJson(core::StringView(buffer)))
    {
        m_stack.push_back({ &m_root, 0 });
    }

    virtual void BeginObject() override
    {
    }

    virtual void EndObject() override
    {
    }

    virtual void BeginField(core::StringView name) override
    {
        const JsonValue::Object& obj = m_stack.back().node->AsObject();
        auto it = obj.find(core::String(name));
        if (it == obj.end())
        {
            SLUG_THROW_EXCEPTION("JsonInputArchive: field not found.");
        }
        m_stack.push_back({ &it->second, 0 });
    }

    virtual void EndField() override
    {
        m_stack.pop_back();
    }

    virtual size_t BeginArray() override
    {
        return m_stack.back().node->AsArray().size();
    }

    virtual void EndArray() override
    {
    }

    virtual void BeginArrayElement() override
    {
        Frame& frame = m_stack.back();
        const JsonValue& elem = frame.node->AsArray()[frame.arrayIndex++];
        m_stack.push_back({ &elem, 0 });
    }

    virtual void EndArrayElement() override
    {
        m_stack.pop_back();
    }

    bool ReadBool()
    {
        return m_stack.back().node->AsBool();
    }

    template<IntegerType T>
    T ReadInteger()
    {
        const JsonValue& node = *m_stack.back().node;
        const int64_t raw = node.IsInteger() ? node.AsInteger() : static_cast<int64_t>(node.AsFloating());
        if constexpr (std::is_enum_v<T>)
        {
            return static_cast<T>(static_cast<std::underlying_type_t<T>>(raw));
        }
        else
        {
            return static_cast<T>(raw);
        }
    }

    template<FloatType T>
    T ReadFloating()
    {
        const JsonValue& node = *m_stack.back().node;
        const double raw = node.IsFloating() ? node.AsFloating() : static_cast<double>(node.AsInteger());
        return static_cast<T>(raw);
    }

    void ReadString(core::String& value)
    {
        value = m_stack.back().node->AsString();
    }

private:
    JsonValue m_root;
    core::TVector<Frame> m_stack;
};

}
