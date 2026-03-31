#pragma once

#include "core/service/ServiceProvider.hpp"
#include "core/tick/TickFunction.hpp"
#include "core/container/List.hpp"

namespace slug::core
{

struct TickGroup
{
    TickGroupType type = {};
    core::TVector<TickFunctionPtr> functions = {};

    bool HasFunction(UUID uuid)
    {
        for (auto& function : functions)
        {
            if (function->uuid == uuid)
            {
                return true;
            }
        }
        return false;
    }
};

class TickManager : public core::IService
{
public:
    SLUG_SERVICE_REGISTER_DEPENDENTS(TickManager, TaskSystem)

    TickManager(core::TaskSystem* taskSystem);
    void Register(TickFunction* function);
    void Execute(TickParam& param);
    bool HasGroup(TickGroupType type);
    bool TryGetGroup(TickGroupType type, TickGroup& outGroup);
    void CleanGroup();
private:
    void CleanFunction(TickGroup& group);
    
private:
    core::Mutex m_groupMutex = {};
    core::TList<TickGroup> m_groups = {};
    core::TaskSystemPtr m_taskSystem = {};
};

}
