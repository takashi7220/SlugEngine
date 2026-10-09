#pragma once
#include "core/container/Vector.hpp"
#include "core/container/UnorderedMap.hpp"
#include "core/thread/Mutex.hpp"
#include "core/memory/ReferencePtr.hpp"

namespace slug::core
{

template<typename Key, typename Value, typename Hash = std::hash<Key>, typename Equal = std::equal_to<Key>>
class ResourcePool : public ReferenceObject
{
public:
    virtual bool Get(const Key& key, Value& value)
    {
        core::UniqueLock lock(m_mutex);
        auto itr = m_table.find(key);
        if (itr == m_table.end())
        {
            return false;
        }
        value = itr->second;
        return true;
    }

    virtual bool Contain(const Key& key)
    {
        core::UniqueLock lock(m_mutex);
        return ContainLocked(key);
    }

    virtual void Add(const Key& key, const Value& value)
    {
        core::UniqueLock lock(m_mutex);
        if (!ContainLocked(key))
        {
            m_table[key] = value;
        }
    }

    virtual void Delete(const Key& key)
    {
        core::UniqueLock lock(m_mutex);
        m_table.erase(key);
    }
private:
    // core::Mutex は非再帰のため、ロック保持中は必ずこちらを使う。
    bool ContainLocked(const Key& key) const
    {
        return m_table.find(key) != m_table.end();
    }

private:
    core::TUnorderedMap<Key, Value, Hash, Equal> m_table;
    core::Mutex m_mutex;
    
};
}
