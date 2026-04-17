#pragma once

#include "core/container/String.hpp"
#include "core/serialize/archiver/archives/json/JsonValue.hpp"

namespace slug::core
{
class JsonParser
{
public:
    explicit JsonParser(core::StringView text)
        : m_text(text)
        , m_pos(0)
    {
    }

    JsonValue Parse()
    {
        SkipWS();
        JsonValue v = ParseValue();
        SkipWS();
        if (m_pos != m_text.size())
        {
            SLUG_THROW_EXCEPTION("JsonParser: unexpected trailing characters.");
        }
        return v;
    }

private:
    core::StringView m_text;
    size_t m_pos;

    void SkipWS()
    {
        while (m_pos < m_text.size() && std::isspace(static_cast<unsigned char>(m_text[m_pos])))
        {
            ++m_pos;
        }
    }

    char Peek() const
    {
        if (m_pos >= m_text.size())
        {
            SLUG_THROW_EXCEPTION("JsonParser: unexpected end of input.");
        }
        return m_text[m_pos];
    }

    char Get()
    {
        if (m_pos >= m_text.size())
        {
            SLUG_THROW_EXCEPTION("JsonParser: unexpected end of input.");
        }
        return m_text[m_pos++];
    }

    bool Consume(char c)
    {
        if (m_pos < m_text.size() && m_text[m_pos] == c)
        {
            ++m_pos;
            return true;
        }
        return false;
    }

    void Expect(char c)
    {
        if (!Consume(c))
        {
            core::String str = core::String("JsonParser: expected '") + c + "'.";
            SLUG_THROW_EXCEPTION(str.c_str());
        }
    }

    JsonValue ParseValue()
    {
        SkipWS();

        char c = Peek();
        if (c == '{')
        {
            return ParseObject();
        }

        if (c == '[')
        {
            return ParseArray();
        }

        if (c == '"')
        {
            return JsonValue(ParseString());
        }

        if (c == 't')
        {
            return ParseTrue();
        }

        if (c == 'f')
        {
            return ParseFalse();
        }

        if (c == 'n')
        {
            return ParseNull();
        }
        if (c == '-' || std::isdigit(static_cast<unsigned char>(c)))
        {
            return ParseNumber();
        }

        SLUG_THROW_EXCEPTION("JsonParser: invalid value.");
    }

    JsonValue ParseObject()
    {
        Expect('{');
        SkipWS();

        JsonValue::Object obj;

        if (Consume('}'))
        {
            return JsonValue(std::move(obj));
        }

        while (true)
        {
            SkipWS();
            core::String key = ParseString();
            SkipWS();
            Expect(':');
            SkipWS();
            JsonValue value = ParseValue();
            obj.emplace(std::move(key), std::move(value));
            SkipWS();

            if (Consume('}'))
            {
                break;
            }
            Expect(',');
            SkipWS();
        }

        return JsonValue(std::move(obj));
    }

    JsonValue ParseArray()
    {
        Expect('[');
        SkipWS();

        JsonValue::Array arr;

        if (Consume(']'))
        {
            return JsonValue(std::move(arr));
        }

        while (true)
        {
            SkipWS();
            arr.push_back(ParseValue());
            SkipWS();

            if (Consume(']'))
            {
                break;
            }
            Expect(',');
            SkipWS();
        }

        return JsonValue(std::move(arr));
    }

    core::String ParseString()
    {
        Expect('"');
        core::String out;

        while (true)
        {
            char c = Get();
            if (c == '"')
            {
                break;
            }

            if (c == '\\')
            {
                char esc = Get();
                switch (esc)
                {
                case '"':
                    out.push_back('"');
                    break;
                case '\\':
                    out.push_back('\\');
                    break;
                case '/':
                    out.push_back('/');
                    break;
                case 'b':
                    out.push_back('\b');
                    break;
                case 'f':
                    out.push_back('\f');
                    break;
                case 'n':
                    out.push_back('\n');
                    break;
                case 'r':
                    out.push_back('\r');
                    break;
                case 't':
                    out.push_back('\t');
                    break;
                default:
                    SLUG_THROW_EXCEPTION("JsonParser: unsupported escape sequence.");
                }
            }
            else
            {
                out.push_back(c);
            }
        }

        return out;
    }

    JsonValue ParseTrue()
    {
        Expect('t');
        Expect('r');
        Expect('u');
        Expect('e');
        return JsonValue(true);
    }

    JsonValue ParseFalse()
    {
        Expect('f');
        Expect('a');
        Expect('l');
        Expect('s');
        Expect('e');
        return JsonValue(false);
    }

    JsonValue ParseNull()
    {
        Expect('n');
        Expect('u');
        Expect('l');
        Expect('l');
        return JsonValue(nullptr);
    }

    JsonValue ParseNumber()
    {
        const size_t start = m_pos;

        if (Consume('-'))
        {
        }

        if (!std::isdigit(static_cast<unsigned char>(Peek())))
        {
            SLUG_THROW_EXCEPTION("JsonParser: invalid number.");
        }

        while (m_pos < m_text.size() && std::isdigit(static_cast<unsigned char>(m_text[m_pos])))
        {
            ++m_pos;
        }

        bool isFloat = false;

        if (m_pos < m_text.size() && m_text[m_pos] == '.')
        {
            isFloat = true;
            ++m_pos;

            while (m_pos < m_text.size() && std::isdigit(static_cast<unsigned char>(m_text[m_pos])))
            {
                ++m_pos;
            }
        }

        if (m_pos < m_text.size() && (m_text[m_pos] == 'e' || m_text[m_pos] == 'E'))
        {
            isFloat = true;
            ++m_pos;

            if (m_pos < m_text.size() && (m_text[m_pos] == '+' || m_text[m_pos] == '-'))
            {
                ++m_pos;
            }

            while (m_pos < m_text.size() && std::isdigit(static_cast<unsigned char>(m_text[m_pos])))
            {
                ++m_pos;
            }
        }

        const core::String numStr(m_text.substr(start, m_pos - start));

        if (isFloat)
        {
            return JsonValue(std::strtod(numStr.c_str(), nullptr));
        }
        return JsonValue(static_cast<std::int64_t>(std::strtoll(numStr.c_str(), nullptr, 10)));
    }
};

inline JsonValue ParseJson(core::StringView text)
{
    return JsonParser(text).Parse();
}
}
