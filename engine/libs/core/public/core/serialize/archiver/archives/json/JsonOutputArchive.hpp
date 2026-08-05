#pragma once

#include "core/serialize/archiver/archives/OutputArchive.hpp"
#include "core/serialize/archiver/archives/json/JsonValue.hpp"
#include "core/container/Vector.hpp"

#include <type_traits>

namespace slug::core
{

class JsonOutputArchive : public OutputArchive<JsonOutputArchive>
{
    struct Frame
    {
        enum class Kind
        {
            Object,
            Array
        };

        Kind kind;
        JsonValue value;
        String pendingKey;
    };

public:
    explicit JsonOutputArchive(core::String& outBuffer, int indentSpaces = 4) noexcept
        : m_buffer(outBuffer)
        , m_indentSpaces(indentSpaces)
    {
    }

    ~JsonOutputArchive()
    {
        m_buffer = m_root.ToString(m_indentSpaces);
    }

    virtual void BeginObject() override
    {
        // オブジェクトフレーム内かつ pendingKey が空 = 基底クラスのインライン展開
        if (!m_stack.empty() &&
            m_stack.back().kind == Frame::Kind::Object &&
            m_stack.back().pendingKey.empty())
        {
            ++m_mergeDepth;
            return;
        }
        m_stack.push_back({ Frame::Kind::Object, JsonValue(JsonValue::Object{}), {} });
    }

    virtual void EndObject() override
    {
        if (m_mergeDepth > 0)
        {
            --m_mergeDepth;
            return;
        }
        JsonValue completed = std::move(m_stack.back().value);
        m_stack.pop_back();
        AttachValue(std::move(completed));
    }

    virtual void BeginField(core::StringView name) override
    {
        m_stack.back().pendingKey = core::String(name);
    }

    virtual void EndField() override
    {
    }

    virtual void BeginArray(size_t) override
    {
        m_stack.push_back({ Frame::Kind::Array, JsonValue(JsonValue::Array{}), {} });
    }

    virtual void EndArray() override
    {
        JsonValue completed = std::move(m_stack.back().value);
        m_stack.pop_back();
        AttachValue(std::move(completed));
    }

    virtual void BeginArrayElement() override
    {
    }

    virtual void EndArrayElement() override
    {
    }

    void WriteBool(bool value)
    {
        AttachValue(JsonValue(value));
    }

    template<IntegerType T>
    void WriteInteger(T value)
    {
        AttachValue(JsonValue(static_cast<int64_t>(value)));
    }

    template<FloatType T>
    void WriteFloating(T value)
    {
        AttachValue(JsonValue(static_cast<double>(value)));
    }

    void WriteString(core::StringView value)
    {
        AttachValue(JsonValue(core::String(value)));
    }

private:
    void AttachValue(JsonValue&& val)
    {
        if (m_stack.empty())
        {
            m_root = std::move(val);
            return;
        }
        Frame& top = m_stack.back();
        if (top.kind == Frame::Kind::Object)
        {
            top.value.AsObject().emplace(top.pendingKey, std::move(val));
        }
        else
        {
            top.value.AsArray().push_back(std::move(val));
        }
    }

private:
    core::String&        m_buffer;
    JsonValue            m_root;
    core::TVector<Frame> m_stack;
    size_t               m_mergeDepth = 0;
    int                  m_indentSpaces;
};

}
