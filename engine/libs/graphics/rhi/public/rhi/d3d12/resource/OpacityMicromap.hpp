#pragma once

#include "rhi/interface/rt/IOpacityMicromap.hpp"
#include "rhi/common/StateTracking.hpp"
#include "rhi/d3d12/resource/DeviceResources.hpp"
#include "rhi/d3d12/resource/Buffer.hpp"

namespace slug::rhi::d3d12
{

class OpacityMicromap : public rt::IOpacityMicromap
{
public:
    SLUG_DECLARE_SOBJECT_TYPEINFO(OpacityMicromap, rt::IOpacityMicromap)
    OpacityMicromap(const Context& context);
    Object GetNativeObject(ObjectType objectType) override;
    const rt::OpacityMicromapDesc& GetDesc() const override;
    bool isCompacted() const override;
    uint64_t GetDeviceAddress() const override;

public:
    core::TReferencePtr<Buffer> dataBuffer;
    rt::OpacityMicromapDesc desc;
    bool allowUpdate = false;
    bool compacted = false;

private:
    [[maybe_unused]] const Context& m_context;
};

}
