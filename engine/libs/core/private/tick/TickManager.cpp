#include "core/tick/TickManager.hpp"

namespace slug::core
{

TickManager::TickManager(core::TaskSystem* taskSystem)
    : m_taskSystem(taskSystem)
{
}

void TickManager::Register(TickFunction* function)
{
    if (function)
    {
        if (function->parent)
        {
            function->parent->DeleteDependency(function->uuid);
        }

        if (!HasGroup(function->type))
        {
            core::UniqueLock lock(m_groupMutex);
            TickGroup& group = m_groups.emplace_back();
            group.type = function->type;
            group.functions.push_back(function);
        }
        else
        {
            TickGroup group;
            if (TryGetGroup(function->type, group))
            {
                core::UniqueLock lock(m_groupMutex);
                if (!group.HasFunction(function->uuid))
                {
                    group.functions.push_back(function);
                }
            }
        }
    }
}

void TickManager::Execute(TickParam& param)
{
    TickGroup group = {};
    if (TryGetGroup(param.groupType, group))
    {
        core::UniqueLock lock(m_groupMutex);
        CleanFunction(group);

        size_t count = group.functions.size();
        if (count > 0)
        {
            param.taskSystem->ParallelFor(0, count, ParallelForOptions(), [&group, param](size_t index)
            {
                group.functions.at(index)->Execute(param);
            });
        }
    }
}

void TickManager::CleanFunction(TickGroup& group)
{
    auto itr = group.functions.begin();
    while (itr != group.functions.end())
    {
        if ((*itr)->GetReferenceCount() <= 1)
        {
            itr = group.functions.erase(itr);
        }
        else if ((*itr)->parent)
        {
            (*itr)->parent->DeleteDependency((*itr)->uuid);
            (*itr)->parent = nullptr;
        }
        else
        {
            itr++;
        }
    }
}

bool TickManager::HasGroup(TickGroupType type)
{
    for (auto& group : m_groups)
    {
        if (group.type == type)
        {
            return true;
        }
    }
    return false;
}

bool TickManager::TryGetGroup(TickGroupType type, TickGroup& outGroup)
{
    for (auto& group : m_groups)
    {
        if (group.type == type)
        {
            outGroup = group;
            return true;
        }
    }
    return false;
}

void TickManager::CleanGroup()
{
    auto itr = m_groups.begin();
    while (itr != m_groups.end())
    {
        if (itr->functions.empty())
        {
            itr = m_groups.erase(itr);
        }
        else
        {
            itr++;
        }
    }
}
}
