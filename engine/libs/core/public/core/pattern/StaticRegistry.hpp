#pragma once

#include "core/container/String.hpp"
#include "core/container/IntrusiveLinkedList.hpp"

namespace slug::core
{

template<typename T>
class TStaticRegistry
{
public:
    typedef T RegistryType;
    typedef IntrusiveLinkedList<RegistryType> LinkedList;
    typedef LinkedList::Node Node;

    static void RegisterNode(TStaticRegistry::Node* node)
    {
        GetStaticRegisteredNodes().push_back(node);
    }

    static TStaticRegistry::LinkedList& GetStaticRegisteredNodes()
    {
        static LinkedList registeredNodes;
        return registeredNodes;
    }
};
}
