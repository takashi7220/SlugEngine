#pragma once

#include "core/utility/NameValuePair.hpp"
#include "core/serialize/common/SerializeTraits.hpp"
#include "core/serialize/common/SerializeUtility.hpp"

namespace slug::core
{
class OutputArchiverTag
{
};

template<typename Archive, typename T>
void DispatchSerialize(Archive& ar, T& value);

template<typename Derived>
class OutputArchive : public OutputArchiverTag
{
public:
    template<typename... Args>
    void operator()(Args&&... args)
    {
        (Process(std::forward<Args>(args)), ...);
    }

    template<typename T>
    void Process(const T& value)
    {
        Derived& self = static_cast<Derived&>(*this);
        DispatchSerialize(self, value)
    }

    template<typename T>
    void Process(NameValuePair<T> nvp)
    {
        Derived& self = static_cast<Derived&>(*this);
        self.BeginNamedValue(nvp.name);
        DispatchSerialize(self, static_cast<const T&>(nvp.value));
        self.EndNamedValue();
    }

    virtual void BeginObject() = 0;
    virtual void EndObject() = 0;

    virtual void BeginField(core::StringView name) = 0;
    virtual void EndField() = 0;

    virtual void BeginArray(size_t size) = 0;
    virtual void EndArray() = 0;

    virtual void BeginArrayElement() = 0;
    virtual void EndArrayElement() = 0;

    virtual void WriteBool(bool value) = 0;

    template<typename T>
    virtual void WriteInteger(T value) = 0;

    template<typename T>
    virtual void WriteFloating(T value) = 0;

    virtual void WriteString(core::StringView value) = 0;
};
}
