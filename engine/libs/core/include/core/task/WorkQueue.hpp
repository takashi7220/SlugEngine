#pragma once

#include "core/task/Task.hpp"
#include "core/container/Queue.hpp"
#include "core/container/Array.hpp"
#include "core/thread/ConditionVariable.hpp"
namespace slug::core
{

inline constexpr uint32_t TaskQueueStarvationQuota = 8;

class WorkQueue
{
public:
    void Push(const TReferencePtr<Task>& task);
    bool TryPop(TReferencePtr<Task>& out);
    bool PopBlocking(TReferencePtr<Task>& out, core::TAtomic<bool>& stopFlag);
    size_t Size();
    void NotifyAll();

private:
    bool EmptyLocked() const noexcept;
    bool PopLocked(TReferencePtr<Task>& out);

private:
    mutable Mutex m_mutex;
    core::ConditionVariable m_conditionVariable;
    core::TArray<core::TQueue<core::TReferencePtr<Task>>, TaskPriorityCount> m_queues;
    core::TArray<uint32_t, TaskPriorityCount> m_skipCounts {};
};
}
