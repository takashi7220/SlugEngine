#include "core/tick/TickObject.hpp"
#include "core/tick/TickManager.hpp"

namespace slug::core
{

TickObject::TickObject(RuntimeID id)
{
    m_primaryTickFunction.reset(NewObject<TickFunction>());
    if (id.IsValid())
    {
        m_primaryTickFunction->id = id;
    }
    m_primaryTickFunction->target = this;
}

void TickObject::AddPrimaryTickDependency(TickObject* object)
{
    if (object)
    {
        m_primaryTickFunction->AddDependency(object->m_primaryTickFunction);
    }
}

void TickObject::DeletePrimaryTickDependency(RuntimeID id)
{
    m_primaryTickFunction->DeleteDependency(id);
}

bool TickObject::HasPrimaryTickDependency(RuntimeID id)
{
    return m_primaryTickFunction->HasDependency(id);
}

const TickFunctionPtr& TickObject::GetPrimaryTickFunction()
{
    return m_primaryTickFunction;
}

}
