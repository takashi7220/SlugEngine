#pragma once

#include "core/utility/NameValuePair.hpp"
#include "core/serialize/common/SerializeTraits.hpp"
#include "core/serialize/common/SerializeUtility.hpp"

namespace slug::core
{
class InputArchiveTag
{
};

template<typename Archive, typename T>
void DispatchDeserialize(Archive& ar, T& value);

template<typename Derived>
class InputArchive : public InputArchiveTag
{
public:
    template<typename... Args>
    void operator()(Args&&... args)
    {
        (Process(std::forward<Args>(args)), ...);
    }

    template<typename T>
    void Process(T& value)
    {
        Derived& self = static_cast<Derived&>(*this);
        DispatchDeserialize(self, value);
    }

    template<typename T>
    void Process(NameValuePair<T> nvp)
    {
        Derived& self = static_cast<Derived&>(*this);
        self.BeginNamedValue(nvp.name);
        DispatchDeserialize(self, nvp.value);
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

    virtual bool ReadBool() = 0;

    template<typename T>
    virtual T ReadInteger() = 0;

    template<typename T>
    virtual T ReadFloating() = 0;

    virtual core::StringView ReadString() = 0;


};
}
