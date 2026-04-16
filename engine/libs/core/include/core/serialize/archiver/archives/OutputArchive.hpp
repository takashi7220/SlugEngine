#pragma once

#include "core/utility/NameValuePair.hpp"
#include "core/utility/BaseClassWrapper.hpp"
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
    template<typename T>
    void Value(const T& value)
    {
        Derived& self = static_cast<Derived&>(*this);
        DispatchSerialize(self, value);
    }

    template<typename T>
    void Field(core::StringView name, const T& value)
    {
        Derived& self = static_cast<Derived&>(*this);
        self.BeginField(name);
        DispatchSerialize(self, value);
        self.EndField();
    }

    template<typename Base>
    void BaseClass(ConstBaseClassWrapper<Base> wrapper)
    {
        Derived& self = static_cast<Derived&>(*this);
        DispatchSerialize(self, static_cast<const Base&>(wrapper.get()));
    }

    virtual void BeginObject() = 0;
    virtual void EndObject() = 0;

    virtual void BeginField(core::StringView name) = 0;
    virtual void EndField() = 0;

    virtual void BeginArray(size_t size) = 0;
    virtual void EndArray() = 0;

    virtual void BeginArrayElement() = 0;
    virtual void EndArrayElement() = 0;
};
}
