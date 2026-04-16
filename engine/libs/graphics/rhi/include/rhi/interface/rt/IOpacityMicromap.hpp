#pragma once

#include "core/container/Vector.hpp"
#include "rhi/ResourceType.hpp"
#include "rhi/interface/IBuffer.hpp"

namespace slug::rhi::rt
{
struct OpacityMicromapDesc 
{
    core::String debug_name = "";
    bool trackLiveness = true;
    OpacityMicromapBuildFlags flags = OpacityMicromapBuildFlags::None;
    core::TVector<OpacityMicromapUsageCount> counts = {};
    IBuffer* inputBuffer = nullptr;
    uint64_t inputBufferOffset = 0;
    IBuffer* perOmmDescs = nullptr;
    uint64_t perOmmDescs_offset = 0;
};

class IOpacityMicromap : public IResource {
public:
    virtual const OpacityMicromapDesc& GetDesc() const = 0;
    virtual bool isCompacted() const = 0;
    virtual uint64_t GetDeviceAddress() const = 0;
};
using OpacityMicromapPtr = core::TReferencePtr<IOpacityMicromap>;
}