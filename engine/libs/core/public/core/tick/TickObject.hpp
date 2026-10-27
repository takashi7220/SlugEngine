#pragma once

#include "core/object/SObject.hpp"
#include "core/tick/TickFunction.hpp"

namespace slug::core
{

class TickObject
{
public:
    TickObject(RuntimeID id = {});
    virtual ~TickObject() = default;
    virtual void Tick(const TickParam& tickParam) = 0;
    void AddPrimaryTickDependency(TickObject* object);
    void DeletePrimaryTickDependency(RuntimeID id);
    bool HasPrimaryTickDependency(RuntimeID id);
    const TickFunctionPtr& GetPrimaryTickFunction();
protected:
    TickFunctionPtr m_primaryTickFunction = {};
};
using TickObjectPtr = core::TReferencePtr<TickObject>;

}
