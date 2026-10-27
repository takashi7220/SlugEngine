#include "core/tick/TickManager.hpp"

namespace slug::core
{

TickManager::TickManager(core::TaskSystem* taskSystem)
    : m_taskSystem(taskSystem)
{
}

void TickManager::Register(TickFunction* function)
{
    if (!function)
    {
        return;
    }

    if (function->parent)
    {
        function->parent->DeleteDependency(function->id);
    }

    core::UniqueLock lock(m_groupMutex);

    // 既存グループは必ず参照で取り出す。コピーに追加すると登録が破棄される。
    for (auto& group : m_groups)
    {
        if (group.type == function->type)
        {
            if (!group.HasFunction(function->id))
            {
                group.functions.push_back(function);
            }
            return;
        }
    }

    TickGroup& newGroup = m_groups.emplace_back();
    newGroup.type = function->type;
    newGroup.functions.push_back(function);
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
            // param を値で捕まえると TickParam::taskSystem のコピーがタスク側に残り、
            // TaskSystem -> WorkQueue -> Task -> ラムダ -> TaskSystem の所有サイクルになる。
            // ParallelFor はタスクの実行完了までは待つがタスクの破棄までは待たないため、
            // 呼び出し側が参照を手放しても TaskSystem がワーカー上で生き続けてしまう。
            param.taskSystem->ParallelFor(0, count, ParallelForOptions(), [&group, &param](size_t index)
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
            (*itr)->parent->DeleteDependency((*itr)->id);
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
