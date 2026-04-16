#pragma once

#include "render/context/BindingCache.hpp"
#include "core/pattern/ResourcePool.hpp"

namespace slug::render
{

struct BindingCacheParamHash
{
    size_t operator()(const BindingCacheParam& param) const
    {
        size_t hash = 0;
        rhi::HashCombine(hash, param.shaderType);
        rhi::HashCombine(hash, param.registerSpace);
        rhi::HashCombine(hash, param.trackLiveness);
        rhi::HashCombine(hash, param.offsetRegisterSpace);
        for (auto& binding : param.bindings)
        {
            rhi::HashCombine(hash, binding.type);
            rhi::HashCombine(hash, binding.slot.slotName);
        }
        return hash;
    }
};

struct BindingCacheParamEqualTo
{
    bool operator()(const BindingCacheParam& lhs, const BindingCacheParam& rhs) const
    {
        if (lhs.bindings.size() != rhs.bindings.size())
        {
            return false;
        }

        bool equal = true;
        equal = equal && (lhs.registerSpace == rhs.registerSpace);
        equal = equal && (lhs.shaderType == rhs.shaderType);
        equal = equal && (lhs.trackLiveness == rhs.trackLiveness);
        equal = equal && (lhs.offsetRegisterSpace == rhs.offsetRegisterSpace);
        for (size_t i = 0; i < lhs.bindings.size(); i++)
        {
            equal = equal && (lhs.bindings.at(i).type == rhs.bindings.at(i).type);
            equal = equal && (lhs.bindings.at(i).slot == rhs.bindings.at(i).slot);
        }

        return equal;
    }
};

class BindingCachePool : public core::ResourcePool<BindingCacheParam, BindingCachePtr, BindingCacheParamHash, BindingCacheParamEqualTo>
{
};
using BindingCachePoolPtr = core::TReferencePtr<BindingCachePool>;

}
