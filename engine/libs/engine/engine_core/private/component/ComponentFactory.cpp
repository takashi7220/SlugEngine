#include "engine_core/component/ComponentFactory.hpp"

namespace slug::engine_core
{
IComponent* ComponentFactory::CreateComponent(Actor* actor, const ComponentInfo& componentInfo)
{
    IComponent* ret = nullptr; 
    GetRegisteredNodes().for_each([&actor, &componentInfo, &ret](DescriptorType* reg){
        if (reg->key == componentInfo.type)
        {
            ret = reg->factory(actor, componentInfo);
            return;
        }
    });
    return ret;
}
}
