#pragma once

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


class Task : public ReferenceObject
{
public:
    using Func = core::TFunctionObject<void()>;
    explicit Task();
    explicit Task(Func _func);
    void Run(TaskSystem& sys);
    bool IsFinished() const noexcept;
    bool TryAddDependent(const core::TReferencePtr<Task>& task);
    core::TFuture<void> GetFuture();

private:
    friend class TaskSystem;
    Func func;
    core::TPromise<void> done;
    core::TAtomic<int32_t> pending { 0 };
    core::TAtomic<bool> finished { false };
    core::TAtomic<bool> started { false };
    core::Mutex m_mutex;
    core::TVector<core::TReferencePtr<Task>> dependents;
};

}
