#pragma once

#include "core/task/Task.hpp"
#include "core/container/Queue.hpp"
#include "core/thread/ConditionVariable.hpp"
namespace slug::core
{
class WorkQueue
{
public:
    void Push(const TReferencePtr<Task>& task);
    bool TryPop(TReferencePtr<Task>& out);
    bool PopBlocking(TReferencePtr<Task>& out, core::TAtomic<bool>& stopFlag);
    size_t Size();
    void NotifyAll();

private:
    mutable Mutex m_mutex;
    core::ConditionVariable m_conditionVariable;
    core::TQueue<core::TReferencePtr<Task>> m_queue;
};
}
