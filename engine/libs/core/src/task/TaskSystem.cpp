#include "core/task/TaskSystem.hpp"
#include <utility>

namespace slug::core
{

TaskSystem::TaskSystem(const Desc& desc)
    : m_desc(desc)
{
    Initialize();
}

TaskSystem::~TaskSystem()
{
    Terminate();
}

void TaskSystem::Initialize()
{
    if (m_initialized)
    {
        return;
    }

    uint32_t workerCount = m_desc.workerCount;
    if (workerCount == 0)
    {
        workerCount = std::max(1u, core::Thread::hardware_concurrency());
    }

    // スレッドを生成
    m_workers.reserve(workerCount);
    for (uint32_t i = 0; i < workerCount; ++i)
    {
        m_workers.emplace_back([this](){
            WorkerMain();
        });
    }
    m_initialized = true;
}

void TaskSystem::WorkerMain()
{
    for (;;)
    {
        // キューに積まれたタスクを順次実行
        TReferencePtr<Task> task;
        if (!m_queue.PopBlocking(task, m_stopFlag))
        {
            if (m_inflight.load(core::MemoryOrderAcquire) == 0)
            {
                break;
            }
            continue;
        }

        if (!task.get())
        {
            continue;
        }

        task->Run(*this);
    }
}

void TaskSystem::Wait(const TaskHandle& task)
{
    task.Wait();
}

bool TaskSystem::Cancel(const TaskHandle& task)
{
    return task.Cancel();
}

TaskHandle TaskSystem::Launch(Task::Func func, std::span<const TaskHandle> prerequisites)
{
    TReferencePtr<Task> newTask = TReferencePtr<Task>(NewObject<Task>(func));
    newTask->pending.store(static_cast<int32_t>(prerequisites.size()), core::MemoryOrderRelease);
    bool shouldCancel = false;

    for (auto& h : prerequisites)
    {
        if (!h.p.get())
        {
            newTask->pending.fetch_sub(1, core::MemoryOrderAcqRel);
            continue;
        }

        if (h.p->IsCanceled())
        {
            newTask->pending.fetch_sub(1, core::MemoryOrderAcqRel);
            shouldCancel = true;
            continue;
        }

        if (h.p->IsFinished())
        {
            newTask->pending.fetch_sub(1, core::MemoryOrderAcqRel);
            continue;
        }

        if (!h.p->TryAddDependent(newTask))
        {
            newTask->pending.fetch_sub(1, core::MemoryOrderAcqRel);
            shouldCancel = shouldCancel || h.p->IsCanceled();
        }
    }

    if (shouldCancel || newTask->IsCanceled())
    {
        newTask->Cancel();
    }
    else if (newTask->pending.load(core::MemoryOrderAcquire) <= 0)
    {
        Enqueue(newTask);
    }

    return TaskHandle(std::move(newTask));
}

void TaskSystem::Enqueue(const TReferencePtr<Task>& t)
{
    if (!t.get())
    {
        return;
    }

    bool expected = false;
    if (!t->queued.compare_exchange_strong(expected, true, core::MemoryOrderAcqRel))
    {
        return;
    }

    m_inflight.fetch_add(1, core::MemoryOrderAcqRel);
    m_queue.Push(t);
}

void TaskSystem::OnTaskFinished() noexcept
{
    const int32_t left = m_inflight.fetch_sub(1, core::MemoryOrderAcqRel) - 1;
    if (left == 0)
    {
        // 全タスクが終了したら、終了条件スレッドを起こす。
        core::ScopedLock lk(m_shutdownMutex);
        m_shutdownCv.notify_all();
    }
}

void TaskSystem::Terminate()
{
    bool expected = false;
    if (!m_stopFlag.compare_exchange_strong(expected, true, core::MemoryOrderAcqRel))
    {
        // 停止済なので返る。
        return;
    }

    // 待機状態のスレッドを開始する。
    m_queue.NotifyAll();
    {
        // 全タスクが完了するまで待つ。
        core::UniqueLock lk(m_shutdownMutex);
        m_shutdownCv.wait(lk, [&] {
            return m_inflight.load(core::MemoryOrderAcquire) == 0;
        });
    }

    // スレッドがまだ動いている場合は、終了まで待つ。
    for (auto& th : m_workers)
    {
        if (th.joinable())
        {
            th.join();
        }
    }
    m_workers.clear();
    m_initialized = false;
}

} // namespace slug::core
