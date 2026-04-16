#pragma once

#include "core/TypeConcept.hpp"
#include "core/container/String.hpp"
#include "core/container/Vector.hpp"

namespace slug::core
{

template<typename Base>
class BaseClassWrapper
{
public:
    explicit BaseClassWrapper(Base& base)
        : m_base(base)
    {
    }

    Base& get() const
    {
        return m_base;
    }
private:
    Base& m_base;
};

template<typename Base>
class ConstBaseClassWrapper
{
public:
    explicit ConstBaseClassWrapper(const Base& baseRef)
        : m_base(baseRef)
    {
    }

    const Base& get() const
    {
        return m_base;
    }

private:
    const Base& m_base;
};

template<typename Base, typename Derived>
BaseClassWrapper<Base> base_class(Derived& derived)
{
    return BaseClassWrapper<Base>(static_cast<Base&>(derived));
}

template<typename Base, typename Derived>
ConstBaseClassWrapper<Base> base_class(const Derived& derived)
{
    return ConstBaseClassWrapper<Base>(static_cast<const Base&>(derived));
}

}
