#include "core/reflection/ReflectionClass.hpp"
#include "core/pattern/Closure.hpp"
#include "core/reflection/Reflection.hpp"

namespace slug
{
namespace core
{

ReflectionClass::ReflectionClass()
    : IPorpertyChanged()
{
}

void ReflectionClass::OnPropertyChanged(const PropertyChangedEventArgs&)
{
}

}
}
