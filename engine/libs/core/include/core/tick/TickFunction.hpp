#pragma once

#include "core/uuid/UUID.hpp"
#include "core/container/Vector.hpp"
#include "core/thread/Atomic.hpp"
#include "core/task/TaskSystem.hpp"

namespace slug::core
{

enum class SystemTickGroupType
{
    Default,
};

class TickObject;
using TickGroupType = core::FixedString<32>;

struct TickParam
{
    double crrentTime = 0.0;
    float deltaTime = 0.0f;
    TickGroupType groupType = {};
    core::TaskSystemPtr taskSystem = nullptr;

    TickParam()
    {
    }
};
class TickManager;

class TickFunction;
using TickFunctionPtr = core::TReferencePtr<TickFunction>;

class TickFunction : public core::ReferenceObject
{
public:
    UUID uuid = {};
    TickGroupType type = SLUG_NAMEOF(SystemTickGroupType::Default);
    TickFunction* parent = nullptr;
    TickObject* target = nullptr;
    core::TVector<TickFunctionPtr> dependents = {};

    double preTickTime = 0.0;
    float tickInterval = 0.0f;

    virtual void Execute(const TickParam& param);
    void AddDependency(TickFunction* function);
    void DeleteDependency(UUID _uuid);
    bool HasDependency(UUID _uuid);

private:
    void ExecuteDependents(const TickParam& param);
    void CleanDependents();
    bool CheckAndUpdateInterval(double currentTime);
};

}
