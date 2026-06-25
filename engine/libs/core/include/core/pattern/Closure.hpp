#pragma once

#include "core/Function.hpp"

namespace slug::core
{

class ClosureUtility
{
public:
    template<class Obj, class R, class... Args>
    static core::TFunctionObject<R(Args...)> MakeClosure(Obj& obj, R(Obj::* mf)(Args...))
    {
        return [&obj, mf](Args... xs) -> R
            {
                return std::invoke(mf, obj, std::forward<Args>(xs)...);
            };
    }

    template<class Obj, class R, class... Args>
    static core::TFunctionObject<R(Args...)> MakeClosure(Obj* obj, R(Obj::* mf)(Args...))
    {
        return [obj, mf](Args... xs) -> R
            {
                return std::invoke(mf, obj, std::forward<Args>(xs)...);
            };
    }

    template<class Obj, class R, class... Args>
    static core::TFunctionObject<R(Args...)> MakeClosure(const Obj& obj, R(Obj::* mf)(Args...) const)
    {
        return [&obj, mf](Args... xs) -> R
            {
                return std::invoke(mf, obj, std::forward<Args>(xs)...);
            };
    }

    template<class Obj, class R, class... Args>
    static core::TFunctionObject<R(Args...)> MakeClosure(const Obj* obj, R(Obj::* mf)(Args...) const)
    {
        return [obj, mf](Args... xs) -> R
            {
                return std::invoke(mf, obj, std::forward<Args>(xs)...);
            };
    }

    template<class Obj, class R, class... Args>
    static core::TFunctionObject<R(Args...)> MakeWeakClosure(const std::shared_ptr<Obj>& sp, R(Obj::* mf)(Args...))
    {
        std::weak_ptr<Obj> w = sp;
        return [w, mf](Args... xs) -> R
            {
                auto s = w.lock();
                if (!s) throw std::bad_function_call {};
                return std::invoke(mf, s.get(), std::forward<Args>(xs)...);
            };
    }

    template<class Obj, class R, class... Args>
    static core::TFunctionObject<R(Args...)> MakeWeakClosure(const std::shared_ptr<const Obj>& sp, R(Obj::* mf)(Args...) const)
    {
        std::weak_ptr<const Obj> w = sp;
        return [w, mf](Args... xs) -> R
            {
                auto s = w.lock();
                if (!s) throw std::bad_function_call {};
                return std::invoke(mf, s.get(), std::forward<Args>(xs)...);
            };
    }
};
}
