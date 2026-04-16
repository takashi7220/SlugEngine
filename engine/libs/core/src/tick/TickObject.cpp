#include "core/tick/TickObject.hpp"
#include "core/tick/TickManager.hpp"

namespace slug::core
{

TickObject::TickObject(UUID uuid)
{
    m_primaryTickFunction.reset(new TickFunction());
    m_primaryTickFunction->uuid = uuid;
    m_primaryTickFunction->target = this;
}

void TickObject::AddPrimaryTickDependency(TickObject* object)
{
    if (object)
    {
        m_primaryTickFunction->AddDependency(object->m_primaryTickFunction);
    }
}

void TickObject::DeletePrimaryTickDependency(UUID uuid)
{
    m_primaryTickFunction->DeleteDependency(uuid);
}

bool TickObject::HasPrimaryTickDependency(UUID uuid)
{
    return m_primaryTickFunction->HasDependency(uuid);
}

const TickFunctionPtr& TickObject::GetPrimaryTickFunction()
{
    return m_primaryTickFunction;
}

}
