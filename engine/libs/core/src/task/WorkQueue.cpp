#include "core/task/WorkQueue.hpp"

namespace slug::core
{
void WorkQueue::Push(const TReferencePtr<Task>& task)
{
    {
        ScopedLock lock(m_mutex);
        m_queue.push(task);
    }
    m_conditionVariable.notify_one();
}
bool WorkQueue::TryPop(TReferencePtr<Task>& out)
{
    ScopedLock lock(m_mutex);
    if (m_queue.empty())
    {
        return false;
    }
    out = std::move(m_queue.front());
    m_queue.pop();
    return true;

}
bool WorkQueue::PopBlocking(TReferencePtr<Task>& out, core::TAtomic<bool>& stopFlag)
{
    UniqueLock lock(m_mutex);

    // Queueに積まれるか、ストップフラグがfalseになるまで待機
    m_conditionVariable.wait(lock, [&]{
        return stopFlag.load() || !m_queue.empty();
    });

    // ストップフラグがtrueかつ、キューが空の場合Popに失敗。
    if (stopFlag.load() && m_queue.empty())
    {
        return false;
    }

    out = std::move(m_queue.front());
    m_queue.pop();
    return true;
}

size_t WorkQueue::Size()
{
    ScopedLock lock(m_mutex);
    return m_queue.size();
}

void WorkQueue::NotifyAll()
{
    m_conditionVariable.notify_all();
}
}
