#pragma once

#include "core/container/String.hpp"
#include "core/container/Vector.hpp"
#include "core/object/SObject.hpp"

/*
* ex.
* SLUG_ATTRIBUTE(slug::core::PropertyDescriptor(slug::core::PropertyEditType::Editable, "Category", "Display Name", "Tooltip", "Group Name"))
*/
#define SLUG_ATTRIBUTE(x)

namespace slug::core
{

using TypeId = uint64_t;

class IAttribute : public SObject
{
public:
    SLUG_DECLARE_SOBJECT_TYPEINFO(IAttribute, SObject)
};
using IAttributePtr = core::TReferencePtr<IAttribute>;

class CustomAttributeList
{
public:
    CustomAttributeList() = default;

    template<typename T>
    CustomAttributeList& AddAttribute(const T& attribute)
    {
        attributes.push_back(core::MakeReference<T>(attribute));
        return *this;
    }

    CustomAttributeList& AddAttribute(const IAttributePtr& attribute)
    {
        attributes.push_back(attribute);
        return *this;
    }

    template<typename T>
    T* GetAttribute()
    {
        for (const auto& attr : attributes)
        {
            if (attr->IsA<T>())
            {
                return core::SObjectUtility::CheckedCast<T>(attr);
            }
        }
        return nullptr;
    }

    template<typename T>
    bool HasAttribute() const
    {
        for (const auto& attr : attributes)
        {
            if (attr->IsA<T>())
            {
                return true;
            }
        }
        return false;
    }

    const core::TVector<IAttributePtr>& GetAttributes() const
    {
        return attributes;
    }
private:
    core::TVector<IAttributePtr> attributes;
};

enum class PropertyEditType : uint8_t
{
    Editable,
    Readonly
};

class PropertyDescriptor : public IAttribute
{
public:
    SLUG_DECLARE_SOBJECT_TYPEINFO(PropertyDescriptor, IAttribute)

    PropertyEditType type;
    core::FixedString<64> category;
    core::FixedString<64> displayName;
    core::FixedString<256> tooltip;
    core::FixedString<64> groupName;

    PropertyDescriptor()
        : type(PropertyEditType::Editable)
        , category("")
        , displayName("")
        , tooltip("")
        , groupName("")
    {
    }

    PropertyDescriptor(PropertyEditType _type, core::StringView _category, core::StringView _displayName, core::StringView _tooltip, core::StringView _groupName)
        : type(_type)
        , category(_category)
        , displayName(_displayName)
        , tooltip(_tooltip)
        , groupName(_groupName)
    {
    }
};
}
