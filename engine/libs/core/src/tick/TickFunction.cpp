#include "core/tick/TickFunction.hpp"
#include "core/tick/TickObject.hpp"
#include "core/tick/TickManager.hpp"
#include "core/debug/Assert.hpp"

namespace slug::core
{

void TickFunction::Execute(const TickParam& param)
{
    if (target != nullptr && CheckAndUpdateInterval(param.crrentTime))
    {
        target->Tick(param);
    }

    CleanDependents();
    ExecuteDependents(param);
}

void TickFunction::AddDependency(TickFunction* function)
{
    if (function && !function->HasDependency(function->uuid))
    {
        if (this->type == function->type)
        {
            if (function->parent)
            {
                function->parent->DeleteDependency(function->uuid);
            }
            function->parent = this;
            dependents.push_back(function);
        }
        else
        {
            SLUG_ASSERT_MSG(false, "Error ");
        }
    }
}

void TickFunction::DeleteDependency(UUID _uuid)
{
    auto itr = dependents.begin();
    while (itr != dependents.end())
    {
        if ((*itr)->uuid == _uuid)
        {
            (*itr)->parent = nullptr;
            itr = dependents.erase(itr);
            break;
        }
        else
        {
            itr++;
        }
    }
}

bool TickFunction::HasDependency(UUID _uuid)
{
    for (auto& itr : dependents)
    {
        if (itr->uuid == _uuid)
        {
            return true;
        }
    }
    return false;
}

void TickFunction::ExecuteDependents(const TickParam& param)
{
    size_t count = dependents.size();
    param.taskSystem->ParallelFor(0, count, ParallelForOptions(), [&](size_t index)
    {
        dependents.at(index)->Execute(param);
    });
}

void TickFunction::CleanDependents()
{
    auto itr = dependents.begin();
    while (itr != dependents.end())
    {
        if ((*itr)->GetReferenceCount() <= 1)
        {
            itr = dependents.erase(itr);
        }
        else
        {
            itr++;
        }
    }
}

bool TickFunction::CheckAndUpdateInterval(double currentTime)
{
    bool enableTick = (currentTime - preTickTime) >= tickInterval;
    preTickTime = currentTime;
    return enableTick;
}

}
