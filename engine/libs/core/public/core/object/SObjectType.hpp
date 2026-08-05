#pragma once

#include <cstdint>

namespace slug::core
{

using SObjectTypeID = uint64_t;

template<typename T>
struct SObjectTypeTag
{
    static char value;
};

template<typename T>
char SObjectTypeTag<T>::value;

template<typename T>
constexpr SObjectTypeID GetSObjectTypeID()
{
    return reinterpret_cast<SObjectTypeID>(&SObjectTypeTag<T>::value);
}

}
