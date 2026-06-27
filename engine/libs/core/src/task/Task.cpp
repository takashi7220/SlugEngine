#include "core/task/Task.hpp"
#include "core/task/TaskSystem.hpp"

#include <exception>
#include <utility>

namespace slug::core
{

Task::Task()
{
}

Task::Task(Func f)
    : func(std::move(f))
{
}

core::TFuture<void> Task::GetFuture()
{
    return done.get_future();
}

bool Task::IsFinished() const noexcept
{
    return finished.load(core::MemoryOrderAcquire);
}

bool Task::IsCanceled() const noexcept
{
    return canceled.load(core::MemoryOrderAcquire);
}

bool Task::TryAddDependent(const TReferencePtr<Task>& task)
{
    core::ScopedLock lk(m_mutex);

    if (IsFinished())
    {
        return false;
    }

    dependents.push_back(task);
    return true;
}

bool Task::Cancel()
{
    if (IsCanceled())
    {
        return true;
    }

    bool expected = false;
    if (!started.compare_exchange_strong(expected, true, core::MemoryOrderAcqRel))
    {
        return IsCanceled();
    }

    canceled.store(true, core::MemoryOrderRelease);
    done.set_value();
    finished.store(true, core::MemoryOrderRelease);

    CancelDependents();
    return true;
}

void Task::CancelDependents()
{
    core::TVector<TReferencePtr<Task>> tmpDependents;
    {
        core::ScopedLock lk(m_mutex);
        tmpDependents.resize(dependents.size());
        for (size_t i = 0; i < dependents.size(); i++)
        {
            tmpDependents.at(i) = dependents.at(i);
        }
        dependents.clear();
    }

    for (auto& dependent : tmpDependents)
    {
        if (!dependent.get())
        {
            continue;
        }

        dependent->Cancel();
    }
}

void Task::ScheduleDependents(TaskSystem& sys)
{
    core::TVector<TReferencePtr<Task>> tmpDependents;
    {
        core::ScopedLock lk(m_mutex);
        tmpDependents.resize(dependents.size());
        for (size_t i = 0; i < dependents.size(); i++)
        {
            tmpDependents.at(i) = dependents.at(i);
        }
        dependents.clear();
    }

    for (auto& dependent : tmpDependents)
    {
        if (!dependent.get() || dependent->IsFinished())
        {
            continue;
        }

        const int32_t left = dependent->pending.fetch_sub(1, core::MemoryOrderAcqRel) - 1;
        if (left <= 0 && !dependent->IsFinished())
        {
            sys.Enqueue(dependent);
        }
    }
}

void Task::Run(TaskSystem& sys)
{
    bool expected = false;
    if (!started.compare_exchange_strong(expected, true, core::MemoryOrderAcqRel))
    {
        sys.OnTaskFinished();
        return;
    }

    try
    {
        if (func)
        {
            func();
        }
        done.set_value();
    }
    catch (...)
    {
        done.set_exception(std::current_exception());
    }

    finished.store(true, core::MemoryOrderRelease);
    ScheduleDependents(sys);

    sys.OnTaskFinished();
}

} // namespace slug::core
