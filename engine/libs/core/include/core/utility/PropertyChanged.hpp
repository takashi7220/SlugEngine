#pragma once

#include "core/Function.hpp"
#include "core/container/String.hpp"
#include "core/container/Vector.hpp"
#include "core/pattern/Closure.hpp"

namespace slug::core
{

struct PropertyChangedEventArgs
{
    core::StringView propertyName;
};

class PropertyChangedEvent
{
public:
    void operator+=(core::TFunctionObject<void(const PropertyChangedEventArgs&)> handler)
    {
        handlers.push_back(handler);
    }

    void operator()(const PropertyChangedEventArgs& args)
    {
        for (auto& h : handlers)
        {
            h(args);
        }
    }

private:
    core::TVector<core::TFunctionObject<void(const PropertyChangedEventArgs&)>> handlers;
};

class IPorpertyChanged
{
public:
    IPorpertyChanged()
    {
        PropertyChanged += ClosureUtility::MakeClosure(this, &IPorpertyChanged::CallPropertyChanged);
    }

    PropertyChangedEvent PropertyChanged;

    virtual void OnPropertyChanged(const PropertyChangedEventArgs& args) = 0;

private:
    void CallPropertyChanged(const PropertyChangedEventArgs& args)
    {
        OnPropertyChanged(args);
    }
};

}
