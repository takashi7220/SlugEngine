#pragma once

#include <cstdint>

#include "core/memory/WeakReferencePtr.hpp"
#include "core/container/String.hpp"
#include "core/container/Vector.hpp"
#include "core/thread/Promise.hpp"
#include "core/Function.hpp"
#include "core/thread/Mutex.hpp"
#include "core/thread/Future.hpp"
#include "core/thread/Thread.hpp"
#include "core/container/Vector.hpp"
#include "core/memory/ReferencePtr.hpp"

namespace slug::core
{

class TaskSystem;
class Task;

enum class TaskPriority : uint8_t
{
    Critical,
    High,
    Normal,
    Low,
    Background,
};

inline constexpr size_t TaskPriorityCount = 5;

class Task : public ReferenceObject
{
public:
    using Func = core::TFunctionObject<void()>;
    explicit Task(TaskPriority _priority = TaskPriority::Normal);
    explicit Task(Func _func, TaskPriority _priority = TaskPriority::Normal);
    void Run(TaskSystem& sys);
    bool IsFinished() const noexcept;
    bool IsCanceled() const noexcept;
    bool Cancel();
    bool TryAddDependent(const core::TReferencePtr<Task>& task);
    core::TFuture<void> GetFuture();
    SLUG_NODISCARD TaskPriority GetPriority() const noexcept;

private:
    friend class TaskSystem;
    void CancelDependents();
    void ScheduleDependents(TaskSystem& sys);

private:
    Func func;
    core::TPromise<void> done;
    core::TAtomic<int32_t> pending { 0 };
    core::TAtomic<bool> finished { false };
    core::TAtomic<bool> canceled { false };
    core::TAtomic<bool> queued { false };
    core::TAtomic<bool> started { false };
    TaskPriority priority;
    core::Mutex m_mutex;
    core::TVector<core::TReferencePtr<Task>> dependents;
};

}
