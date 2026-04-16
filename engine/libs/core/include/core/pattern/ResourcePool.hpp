#pragma once
#include "core/container/Vector.hpp"
#include "core/container/UnorderedMap.hpp"
#include "core/thread/Mutex.hpp"

namespace slug::core
{

template<typename Key, typename Value, typename Hash = std::hash<Key>, typename Equal = std::equal_to<Key>>
class ResourcePool : public ReferenceObject
{
public:
    virtual bool Get(const Key& key, Value& value)
    {
        core::UniqueLock lock(m_mutex);
        if (Contain(key))
        {
            value = m_table[key];
            return true;
        }
        return false;
    }

    virtual bool Contain(const Key& key)
    {
        core::UniqueLock lock(m_mutex);
        if (m_table.find(key) != m_table.end())
        {
            return true;
        }
        return false;
    }

    virtual void Add(const Key& key, const Value& value)
    {
        core::UniqueLock lock(m_mutex);
        if (!Contain(key))
        {
            m_table[key] = value;
        }
    }

    virtual void Delete(const Key& key)
    {
        core::UniqueLock lock(m_mutex);
        if (Contain(key))
        {
            m_table.erase(key);
        }
    }
private:
    core::TUnorderedMap<Key, Value, Hash, Equal> m_table;
    core::Mutex m_mutex;
    
};
}
