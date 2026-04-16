#pragma once

#include <d3d12.h>
#include "core/thread/Mutex.hpp"
#include "rhi/interface/rt/IAccelStruct.hpp"
#include "rhi/d3d12/interface/IDescriptorHeap.hpp"
#include "rhi/d3d12/resource/Buffer.hpp"
#include "rhi/d3d12/resource/Context.hpp"

namespace slug::rhi::d3d12
{

class AccelStruct : public rt::IAccelStruct
{
public:
    AccelStruct(const Context& context);
    virtual ~AccelStruct() override;
    void CreateSRV(size_t descriptor) const;
    Object GetNativeObject(ObjectType objectType) override;
    const rt::AccelStructDesc& GetDesc() const override;
    bool isCompacted() const override;
    uint64_t GetDeviceAddress() const override;

public:
    core::TReferencePtr<d3d12::Buffer> dataBuffer;
    core::TVector<rt::AccelStructPtr> bottomLevelAses;
    core::TVector<D3D12_RAYTRACING_INSTANCE_DESC> dxrInstances;
    rt::AccelStructDesc desc;
    bool allowUpdate = false;
    bool compacted = false;
    size_t rtxmuId = ~0ull;

private:
    const Context& m_context;

};

}
