#pragma once

#include "core/reflection/Type.hpp"
#include "core/utility/Hash.hpp"
#include "core/container/String.hpp"
#include "core/container/UnorderedMap.hpp"
#include "core/Function.hpp"

namespace slug::core
{
class ReflectionRegistry
{
public:
    template<typename T>
    Type& RegisterType(core::StringView name)
    {
        TypeId id = core::HashUtility::ConvertStringToHash64(name);
        auto itr = m_types.find(id);
        if (itr != m_types.end())
        {
            return itr->second;
        }
        m_types[id] = Type(id, name);
        return m_types[id];
    }

    Type* FindType(TypeId id)
    {
        auto it = m_types.find(id);
        if (it == m_types.end())
        {
            return nullptr;
        }
        return &it->second;
    }

    const Type* FindType(TypeId id) const
    {
        auto it = m_types.find(id);
        if (it == m_types.end())
        {
            return nullptr;
        }
        return &it->second;
    }

    const Type* FindType(const char* name) const
    {
        return FindType(core::HashUtility::ConvertStringToHash64(name));
    }

    void Clear()
    {
        m_types.clear();
    }

private:
    core::TUnorderedMap<TypeId, Type> m_types;
};

}
