#pragma once

#include "core/container/UnorderedMap.hpp"
#include "core/container/Vector.hpp"
#include "core/container/String.hpp"
#include <variant>

namespace slug::core
{

struct JsonValue
{
    using Object = core::TUnorderedMap<core::String, JsonValue>;
    using Array = core::TVector<JsonValue>;
    using Variant = std::variant<
        std::nullptr_t,
        bool,
        int64_t,
        double,
        core::String,
        Array,
        Object>;

    Variant value;

    JsonValue()
        : value(nullptr)
    {
    }

    JsonValue(std::nullptr_t)
        : value(nullptr)
    {
    }

    JsonValue(bool v)
        : value(v)
    {
    }

    JsonValue(int64_t v)
        : value(v)
    {
    }

    JsonValue(double v)
        : value(v)
    {
    }

    JsonValue(const core::String& v)
        : value(v)
    {
    }

    JsonValue(core::String&& v)
        : value(std::move(v))
    {
    }

    JsonValue(const char* v)
        : value(core::String(v))
    {
    }

    JsonValue(const Array& v)
        : value(v)
    {
    }

    JsonValue(Array&& v)
        : value(std::move(v))
    {
    }

    JsonValue(const Object& v) : value(v)
    {
    }

    JsonValue(Object&& v)
        : value(std::move(v))
    {
    }

    bool IsNull()   const
    {
        return std::holds_alternative<std::nullptr_t>(value);
    }

    bool IsBool()   const
    {
        return std::holds_alternative<bool>(value);
    }

    bool IsInteger()    const
    {
        return std::holds_alternative<std::int64_t>(value);
    }
    bool IsFloating() const
    {
        return std::holds_alternative<double>(value);
    }
    bool IsString() const
    {
        return std::holds_alternative<core::String>(value);
    }
    bool IsArray()  const
    {
        return std::holds_alternative<Array>(value);
    }
    bool IsObject() const
    {
        return std::holds_alternative<Object>(value);
    }

    bool& AsBool()
    {
        return std::get<bool>(value);
    }
    const bool& as_bool() const
    {
        return std::get<bool>(value);
    }

    int64_t& AsInteger()
    {
        return std::get<std::int64_t>(value);
    }
    const int64_t& AsInteger() const
    {
        return std::get<std::int64_t>(value);
    }

    double& AsFloating()
    {
        return std::get<double>(value);
    }
    const double& AsFloating() const
    {
        return std::get<double>(value);
    }

    core::String& AsString()
    {
        return std::get<core::String>(value);
    }

    const core::String& AsString() const
    {
        return std::get<core::String>(value);
    }

    Array& AsArray()
    {
        return std::get<Array>(value);
    }
    const Array& AsArray() const
    {
        return std::get<Array>(value);
    }

    Object& AsObject()
    {
        return std::get<Object>(value);
    }
    const Object& AsObject() const
    {
        return std::get<Object>(value);
    }

    static core::String EscapeString(const core::String& s)
    {
        core::String out;
        out.reserve(s.size());

        for (char c : s)
        {
            switch (c)
            {
            case '\"': out += "\\\""; break;
            case '\\': out += "\\\\"; break;
            case '\b': out += "\\b"; break;
            case '\f': out += "\\f"; break;
            case '\n': out += "\\n"; break;
            case '\r': out += "\\r"; break;
            case '\t': out += "\\t"; break;
            default: out += c; break;
            }
        }

        return out;
    }

    core::String ToString() const
    {
        if (IsNull())
        {
            return "null";
        }

        if (IsBool())
        {
            return as_bool() ? "true" : "false";
        }

        if (IsInteger())
        {
            return core::StringUtility::ToString(AsInteger());
        }

        if (IsFloating())
        {
            core::String str = {};
            core::StringUtility::Sprintfs(str, "%.17g", AsFloating());
            return str;
        }

        if (IsString())
        {
            return "\"" + EscapeString(AsString()) + "\"";
        }

        if (IsArray())
        {
            core::String out = "[";
            bool first = true;
            for (const auto& e : AsArray())
            {
                if (!first)
                {
                    out += ",";
                }
                first = false;
                out += e.ToString();
            }
            out += "]";
            return out;
        }

        core::String out = "{";
        bool first = true;
        for (const auto& [k, v] : AsObject())
        {
            if (!first)
            {
                out += ",";
            }
            first = false;
            out += "\"" + EscapeString(k) + "\":" + v.ToString();
        }
        out += "}";
        return out;
    }
};

}
