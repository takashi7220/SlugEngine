#include "core/task/WorkQueue.hpp"

namespace slug::core
{
void WorkQueue::Push(const TReferencePtr<Task>& task)
{
    {
        ScopedLock lock(m_mutex);
        m_queues[static_cast<size_t>(task->GetPriority())].push(task);
    }
    m_conditionVariable.notify_one();
}

bool WorkQueue::EmptyLocked() const noexcept
{
    for (const auto& q : m_queues)
    {
        if (!q.empty())
        {
            return false;
        }
    }
    return true;
}

bool WorkQueue::PopLocked(TReferencePtr<Task>& out)
{
    // クォータに達した飢餓状態のレベルがあれば、優先度を無視して先に処理する。
    for (size_t level = 0; level < TaskPriorityCount; ++level)
    {
        if (m_skipCounts[level] >= TaskQueueStarvationQuota && !m_queues[level].empty())
        {
            out = std::move(m_queues[level].front());
            m_queues[level].pop();
            m_skipCounts[level] = 0;
            return true;
        }
    }

    for (size_t level = 0; level < TaskPriorityCount; ++level)
    {
        if (m_queues[level].empty())
        {
            continue;
        }

        out = std::move(m_queues[level].front());
        m_queues[level].pop();

        for (size_t lower = level + 1; lower < TaskPriorityCount; ++lower)
        {
            if (!m_queues[lower].empty())
            {
                ++m_skipCounts[lower];
            }
        }
        return true;
    }

    return false;
}

bool WorkQueue::TryPop(TReferencePtr<Task>& out)
{
    ScopedLock lock(m_mutex);
    return PopLocked(out);
}

bool WorkQueue::PopBlocking(TReferencePtr<Task>& out, core::TAtomic<bool>& stopFlag)
{
    UniqueLock lock(m_mutex);

    // Queueに積まれるか、ストップフラグがfalseになるまで待機
    m_conditionVariable.wait(lock, [&]{
        return stopFlag.load() || !EmptyLocked();
    });

    // ストップフラグがtrueかつ、キューが空の場合Popに失敗。
    if (stopFlag.load() && EmptyLocked())
    {
        return false;
    }

    return PopLocked(out);
}

size_t WorkQueue::Size()
{
    ScopedLock lock(m_mutex);
    size_t total = 0;
    for (const auto& q : m_queues)
    {
        total += q.size();
    }
    return total;
}

void WorkQueue::NotifyAll()
{
    m_conditionVariable.notify_all();
}
}
