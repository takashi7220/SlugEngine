#pragma once

#include "core/utility/NameValuePair.hpp"
#include "core/utility/BaseClassWrapper.hpp"
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
    template<typename T>
    void Value(T& value)
    {
        Derived& self = static_cast<Derived&>(*this);
        DispatchDeserialize(self, value);
    }

    template<typename T>
    void Field(core::StringView name, T& value)
    {
        Derived& self = static_cast<Derived&>(*this);
        self.BeginField(name);
        DispatchDeserialize(self, value);
        self.EndField();
    }

    template<typename Base>
    void BaseClass(BaseClassWrapper<Base> wrapper)
    {
        Derived& self = static_cast<Derived&>(*this);
        DispatchDeserialize(self, static_cast<Base&>(wrapper.get()));
    }

    virtual void BeginObject() = 0;
    virtual void EndObject() = 0;

    virtual void BeginField(core::StringView name) = 0;
    virtual void EndField() = 0;

    virtual size_t BeginArray() = 0;
    virtual void EndArray() = 0;

    virtual void BeginArrayElement() = 0;
    virtual void EndArrayElement() = 0;
};
}
