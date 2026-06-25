#pragma once

#include "core/Function.hpp"

namespace slug::core
{
template<typename T>
class IntrusiveLinkedList
{
public:
    struct Node
    {
        Node* next = nullptr;
        Node* prev = nullptr;
        T* data = nullptr;
    };

    IntrusiveLinkedList()
        : head(nullptr)
        , tail(nullptr)
    {
    }

    void push_back(Node* node)
    {
        if (!head)
        {
            head = node;
            tail = node;
        }
        else
        {
            tail->next = node;
            node->prev = tail;
            tail = node;
        }
    }

    void remove(Node* node)
    {
        if (node->prev)
        {
            node->prev->next = node->next;
        }
        else
        {
            head = node->next;
        }
        if (node->next)
        {
            node->next->prev = node->prev;
        }
        else
        {
            tail = node->prev;
        }
        node->next = nullptr;
        node->prev = nullptr;
    }

    void clear()
    {
        Node* current = head;
        while (current)
        {
            Node* next = current->next;
            current->next = nullptr;
            current->prev = nullptr;
            current = next;
        }
        head = nullptr;
        tail = nullptr;
    }

    void for_each(core::TFunctionObject<void(T*)> func)
    {
        Node* current = head;
        while (current)
        {
            func(current->data);
            current = current->next;
        }
    }

    void for_each_reverse(core::TFunctionObject<void(T*)> func)
    {
        Node* current = tail;
        while (current)
        {
            func(current->data);
            current = current->prev;
        }
    }

    void for_each(core::TFunctionObject<void(T*)> func) const
    {
        Node* current = head;
        while (current)
        {
            func(current->data);
            current = current->next;
        }
    }

    void for_each_reverse(core::TFunctionObject<void(T*)> func) const
    {
        Node* current = tail;
        while (current)
        {
            func(current->data);
            current = current->prev;
        }
    }

private:
    Node* head;
    Node* tail;
};
}
