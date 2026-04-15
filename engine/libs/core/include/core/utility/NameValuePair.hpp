#pragma once

#include "core/container/String.hpp"

namespace slug::core
{
template<typename T>
struct NameValuePair
{
    core::StringView name;
    T& value;

    NameValuePair(core::StringView _name, T& _value)
        : name(_name)
        , value(_value)
    {
    }
};

template<typename T>
struct ConstNameValuePair
{
    core::StringView name;
    const T& value;

    NameValuePair(core::StringView _name, const T& _value)
        : name(_name)
        , value(_value)
    {
    }
};

template<typename T>
inline NameValuePair<T> MakeNameValuePair(core::StringView name, T& value)
{
    return NameValuePair<T>(name, value);
}

template<typename T>
inline ConstNameValuePair<T> MakeNameValuePair(const char* name, const T& value)
{
    return ConstNameValuePair<T>(name, value);
}
}
