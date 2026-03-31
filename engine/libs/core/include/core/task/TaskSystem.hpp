#pragma once

#include "math/Math.hpp"
#include "core/task/Task.hpp"
#include "core/task/WorkQueue.hpp"
#include "core/container/Vector.hpp"
#include "core/service/ServiceProvider.hpp"
#include "core/memory/MemoryUtilities.hpp"

namespace slug::core
{
struct TaskHandle
{
    TReferencePtr<Task> p;

    TaskHandle() = default;

    explicit TaskHandle(TReferencePtr<Task> t)
        : p(std::move(t))
    {}

    bool Valid() const noexcept
    {
        return p.get() != nullptr;
    }

    void Wait() const
    {
        if (!p.get())
        {
            return;
        }
        p->GetFuture().wait();
    }
};

struct ParallelForOptions
{
    size_t grain = 0;

    ParallelForOptions()
    {
    }

    ParallelForOptions(size_t _grain)
        : grain(_grain)
    {
    }
};

class TaskSystem : public IService
{
public:
    SLUG_SERVICE_CUSTOM_REGISTER(slug::core::TaskSystem)

    constexpr static uint32_t GRAIN_BIAS_RATE = 4;
    struct Desc
    {
        uint32_t workerCount = 0;
    };

    explicit TaskSystem(const Desc& desc = {});
    ~TaskSystem();

    TaskSystem(const TaskSystem&) = delete;
    TaskSystem& operator=(const TaskSystem&) = delete;

    void Initialize();
    void Terminate();
    void Wait(const TaskHandle& task);

    TaskHandle Launch(Task::Func func, std::span<const TaskHandle> prerequisites = {});
    void Enqueue(const TReferencePtr<Task>& t);
    void OnTaskFinished() noexcept;

    template<typename FuncType>
    core::TFuture<void> ParallelForAsync(size_t begin, size_t end, const ParallelForOptions& options, FuncType body)
    {
        if (begin > end)
        {
            core::TPromise<void> p;
            auto f = p.get_future();
            p.set_value();
            return f;
        }

        size_t grain = (options.grain == 0) ? m_desc.workerCount * GRAIN_BIAS_RATE : options.grain;
        const size_t count = end - begin;
        const size_t numChunks = (count + grain - 1) / grain;

        // 実行完了待ちのタスクを生成
        TReferencePtr<Task> joinTask(new Task());
        joinTask->pending.store(static_cast<int32_t>(numChunks), std::memory_order_relaxed);
        core::TFuture<void> future = joinTask->done.get_future();

        // 実行完了待ちタスクを実行タスクに依存させる。
        // 実行タスクが全て実行されれば、実行完了待ちタスクが実行されWaitが解除される。
        for (size_t c = 0; c < numChunks; ++c)
        {
            const size_t chunkBegin = begin + c * grain;
            const size_t chunkEnd = math::TMin(end, chunkBegin + grain);

            TReferencePtr<Task> t = new Task([=](){
                 for (size_t i = chunkBegin; i < chunkEnd; ++i)
                 {
                     body(i);
                 }
            });

            t->TryAddDependent(joinTask);
            t->pending.store(0, std::memory_order_relaxed);
            Enqueue(t);
        }
        return future;
    }

    template<typename FuncType>
    void ParallelFor(size_t begin, size_t end, const ParallelForOptions& options, FuncType body)
    {
        core::TFuture<void> future = ParallelForAsync(begin, end, options, body);
        future.get();
    }

private:
    void WorkerMain();

private:
    Desc m_desc;
    bool m_initialized = false;
    core::TAtomic<bool> m_stopFlag { false };
    core::TAtomic<int32_t> m_inflight { 0 };

    Mutex m_shutdownMutex;
    ConditionVariable m_shutdownCv;

    WorkQueue m_queue;
    core::TVector<core::Thread> m_workers;
};
using TaskSystemPtr = TReferencePtr<TaskSystem>;

}
