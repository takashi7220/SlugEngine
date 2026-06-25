#pragma once

#include "core/object/SObject.hpp"
#include "core/tick/TickFunction.hpp"

namespace slug::core
{

class TickObject
{
public:
    TickObject(UUID uuid = {});
    virtual ~TickObject() = default;
    virtual void Tick(const TickParam& tickParam) = 0;
    void AddPrimaryTickDependency(TickObject* object);
    void DeletePrimaryTickDependency(UUID uuid);
    bool HasPrimaryTickDependency(UUID uuid);
    const TickFunctionPtr& GetPrimaryTickFunction();
protected:
    TickFunctionPtr m_primaryTickFunction = {};
};
using TickObjectPtr = core::TReferencePtr<TickObject>;

}
