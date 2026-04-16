#pragma once

#include "rhi/Rhi.hpp"
#include "rhi/d3d12/D3D12.hpp"
#include "rhi/d3d12/resource/BindingLayout.hpp"
#include "rhi/d3d12/resource/DeviceResources.hpp"
#include "rhi/d3d12/resource/Context.hpp"

namespace slug::rhi::d3d12
{

class BindingSet : public IBindingSet
{
public:
	BindingSet(const Context& context, DeviceResources& resources);
	virtual ~BindingSet() override;
	void CreateDescriptors();
    const BindingSetDesc* GetDesc() const override;
    IBindingLayout* GetLayout() const override;

public:
	core::TReferencePtr<BindingLayout> layout;
	BindingSetDesc desc;
	DescriptorIndex descriptorTableSrvEtc = 0;
	DescriptorIndex descriptorTableSamplers = 0;
	RootParameterIndex rootParameterIndexSrvEtc = 0;
	RootParameterIndex rootParameterIndexSamplers = 0;
	bool descriptorTableValidSrvEtc = false;
	bool descriptorTableValidSamplers = false;
	bool hasUavBindings = false;
	core::TStaticVector<core::TPair<RootParameterIndex, IBuffer*>, MAX_VOLATILE_CONSTANT_BUFFERS_PER_LAYOUT> rootParametersVolatileCb;
	core::TVector<core::TReferencePtr<IResource>> resources;
	core::TVector<uint16_t> bindingsThatNeedTransitions;

private:
	const Context& m_context;
	DeviceResources& m_resources;
};
}
