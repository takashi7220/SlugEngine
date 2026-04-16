#pragma once

#include "core/pack/PackFile.hpp"
#include "core/pattern/ResourcePool.hpp"


namespace slug::core
{

class PackFilePool : public ResourcePool<PackFilePath, PackFilePtr, PackFilePathKeyHash, PackFilePathKeyCompare>
{
public:
    PackFilePool() = default;
};
using PackFilePoolPtr = core::TReferencePtr<PackFilePool>;

}
