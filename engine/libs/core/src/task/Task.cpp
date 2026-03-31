#include "core/task/Task.hpp"
#include "core/task/TaskSystem.hpp"

#include <utility>
#include <exception>

namespace slug::core
{

Task::Task()
{

}

Task::Task(Func f)
    : func(std::move(f))
{}

core::TFuture<void> Task::GetFuture()
{
    return done.get_future();
}

bool Task::IsFinished() const noexcept
{
    return finished.load(std::memory_order_acquire);
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

void Task::Run(TaskSystem& sys)
{
    {
        // スタートフラグを立てる、立てるのに失敗 = スタート済なので即時変える。
        bool expected = false;
        if (!started.compare_exchange_strong(expected, true, std::memory_order_acq_rel))
        {
            return;
        }
    }

    try
    {
        // funcを実行
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

    // 終了フラグをセット
    finished.store(true, std::memory_order_release);

    // 一時配列に依存タスクを設定
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

    // 依存タスクの実行を試みる
    for (auto& dpendent : tmpDependents)
    {
        if (!dpendent.get())
        {
            continue;
        }

        // 保留数が0になった時、タスクに積む
        const int32_t prev = dpendent->pending.fetch_sub(1, std::memory_order_acq_rel) - 1;
        if (prev <= 0)
        {
            sys.Enqueue(dpendent);
        }
    }

    sys.OnTaskFinished();
}

} // namespace slug::core
