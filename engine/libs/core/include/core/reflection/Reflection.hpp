#include "core/reflection/Type.hpp"
#include "core/reflection/Property.hpp"
#include "core/reflection/Constructor.hpp"
#include "core/reflection/Instance.hpp"
#include "core/reflection/Method.hpp"
#include "core/reflection/Variant.hpp"
#include "core/reflection/ReflectionUtility.hpp"
#include "core/reflection/Registration.hpp"
#include "core/reflection/Attribute.hpp"

#define SLUG_REFLECTION(...)

#if defined(SLUG_USE_RTTR)
namespace slug::core
{
using Constructor = rttr::constructor;
using Instance = rttr::instance;
using Method = rttr::method;
using Property = rttr::property;
using Registration = rttr::registration;
using Type = rttr::type;
using Variant = rttr::variant;

}
#endif
