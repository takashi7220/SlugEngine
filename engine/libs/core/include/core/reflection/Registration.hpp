#pragma once
#include "core/container/String.hpp"
#include "core/Function.hpp"

namespace slug::core
{
#if 0
template<typename T>
using SetterFunc = TFunctionObject<void(const T)>;

template<typename T>
using GetterFunc = TFunctionObject<T()>;

class Registration
{
public:
    template<typename ClassType>
    class Class
    {
    public:
        Class(core::StringView className)
        {
        }

        virtual ~Class()
        {
        }

        template<typename... Args>
        void Constructor()
        {
        }

        template<typename MemberType>
        void Property(core::StringView propertyName, SetterFunc setter, GetterFunc getter)
        {
        }

        template<typename RetType, typename... Args>
        void Method(core::StringView methodName, TFunctionObject<RetType(Args)> method)
        {
        }
    private:
        core::String className;
        core::TVector<core::String> propertyNames;
        core::TVector<core::String> methodNames;

    };

};
#endif
}
