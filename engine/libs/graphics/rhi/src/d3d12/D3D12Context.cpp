#include "rhi/interface/IContext.hpp"
#include "core/memory/ReferencePtr.hpp"
#include "rhi/Rhi.hpp"
#include "rhi/d3d12/D3D12.hpp"
#include "rhi/d3d12/resource/Device.hpp"
#include "rhi/common/Resource.hpp"
#include "core/Function.hpp"
#include <dxgi1_5.h>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib ")

namespace slug
{
namespace rhi
{
class SwapChainD3D12 : public core::ReferenceObject
{
public:
    static SwapChainD3D12* Create(const IContext::WindowParam& param, core::TReferencePtr<ID3D12CommandQueue>& graphicsQueue, bool enableDebug);
    static bool CheckAllowTearingSupport(core::TReferencePtr<IDXGIFactory2>& factory);

    bool Resize(rhi::DevicePtr& device, uint32_t width, uint32_t height);
    bool CreateRenderTargets(rhi::DevicePtr& device);
    bool CreateFenceEvent();
    void ReleaseRenderTargets(rhi::DevicePtr& device);
    void Present(uint32_t syncInterval, core::TReferencePtr<ID3D12Fence>& fence, core::TReferencePtr<ID3D12CommandQueue>& graphicsQueue);
    uint32_t GetCurrentBackBufferIndex();
    uint32_t GetBackBufferCount();
    TexturePtr GetBackBuffer(uint32_t index);

    core::StringView GetName();

private:
    DXGI_SWAP_CHAIN_DESC GetSwapChainDesc();
    DXGI_SWAP_CHAIN_FULLSCREEN_DESC GetFullScreenDesc();

private:
    core::FixedString<256> m_name;
    core::TReferencePtr<IDXGISwapChain3> m_swapChain = nullptr;
    core::TVector<core::TReferencePtr<ID3D12Resource>> m_swapChainBuffers = {};
    core::TVector<rhi::TexturePtr> m_rhiSwapChainBuffers = {};
    core::TVector<HANDLE> m_frameFenceEvents = {};
    uint32_t m_frameCount = 0;
    bool m_tearingSupport = true;
    bool m_vcync = true;
    HWND m_windowHandle = {};
};
using SwapChainD3D12Ptr = core::TReferencePtr<SwapChainD3D12>;

SwapChainD3D12* SwapChainD3D12::Create(const IContext::WindowParam& param, core::TReferencePtr<ID3D12CommandQueue>& graphicsQueue, bool enableDebug)
{
    SwapChainD3D12* swapChain(new SwapChainD3D12());
    swapChain->m_name = param.name;
    swapChain->m_windowHandle = (HWND)param.handle;

    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
    ZeroMemory(&swapChainDesc, sizeof(swapChain->m_swapChain));
    swapChainDesc.Width = (uint32_t)param.width;
    swapChainDesc.Height = (uint32_t)param.height;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;
    swapChainDesc.BufferUsage = DXGI_USAGE_SHADER_INPUT | DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.BufferCount = 3;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    swapChainDesc.Format = DXGI_FORMAT_R10G10B10A2_UNORM;

    core::TReferencePtr<IDXGIFactory2> pDxgiFactory = nullptr;
    HRESULT hr = CreateDXGIFactory2(enableDebug ? DXGI_CREATE_FACTORY_DEBUG : 0, IID_PPV_ARGS(&pDxgiFactory));
    if (FAILED(hr))
    {
        core::String str = core::StringUtility::Sprintfs("Error : CreateDXGIFactory2 hr = %x", hr);
        SLUG_ASSERT_MSG(false, str.c_str());
        return nullptr;
    }

    if (CheckAllowTearingSupport(pDxgiFactory))
    {
        swapChainDesc.Flags |= DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;
    }

    DXGI_SWAP_CHAIN_FULLSCREEN_DESC fullscreenDesc = {};
    fullscreenDesc.RefreshRate.Numerator = 1;
    fullscreenDesc.RefreshRate.Denominator = 60;
    fullscreenDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE;
    fullscreenDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    fullscreenDesc.Windowed = true;

    core::TReferencePtr<IDXGISwapChain1> pSwapChain1;
    hr = pDxgiFactory->CreateSwapChainForHwnd(graphicsQueue, swapChain->m_windowHandle, &swapChainDesc, &fullscreenDesc, nullptr, &pSwapChain1);
    if (FAILED(hr))
    {
        return nullptr;
    }

    hr = pSwapChain1->QueryInterface(IID_PPV_ARGS(&swapChain->m_swapChain));
    if (FAILED(hr))
    {
        return nullptr;
    }
    return swapChain;
}


bool SwapChainD3D12::CheckAllowTearingSupport(core::TReferencePtr<IDXGIFactory2>& factory)
{
    core::TReferencePtr<IDXGIFactory5> pDxgiFactory5;
    if (SUCCEEDED(factory->QueryInterface(IID_PPV_ARGS(&pDxgiFactory5))))
    {
        BOOL supported = 0;
        if (SUCCEEDED(pDxgiFactory5->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &supported, sizeof(supported))))
        {
            return (supported != 0);
        }
    }
    return false;
}

bool SwapChainD3D12::Resize(rhi::DevicePtr& device, uint32_t width, uint32_t height)
{
    ReleaseRenderTargets(device);
    DXGI_SWAP_CHAIN_DESC swapChainDesc = GetSwapChainDesc();
    HRESULT hr = m_swapChain->ResizeBuffers(swapChainDesc.BufferCount, width, height, swapChainDesc.BufferDesc.Format, swapChainDesc.Flags);
    if (FAILED(hr))
    {
        return false;
    }

    return CreateRenderTargets(device);
}

bool SwapChainD3D12::CreateRenderTargets(rhi::DevicePtr& device)
{
    DXGI_SWAP_CHAIN_DESC swapChainDesc = GetSwapChainDesc();
    m_swapChainBuffers.resize(swapChainDesc.BufferCount);
    m_rhiSwapChainBuffers.resize(swapChainDesc.BufferCount);

    for (UINT i = 0; i < swapChainDesc.BufferCount; i++)
    {
        const HRESULT hr = m_swapChain->GetBuffer(i, IID_PPV_ARGS(&m_swapChainBuffers[i]));
        if (FAILED(hr))
        {
            return false;
        }

        rhi::TextureDesc textureDesc;
        textureDesc.width = swapChainDesc.BufferDesc.Width;
        textureDesc.height = swapChainDesc.BufferDesc.Height;
        textureDesc.sampleCount = 1;
        textureDesc.sampleQuality = 0;
        textureDesc.format = rhi::FormatType::R10G10B10A2_UNORM;
        textureDesc.debugName = "SwapChainBuffer";
        textureDesc.renderTarget = true;
        textureDesc.uav = false;
        textureDesc.initialState = rhi::ResourceStates::Present;
        textureDesc.keepInitialState = true;

        m_rhiSwapChainBuffers[i] = device->CreateHandleForNativeTexture(rhi::object_types::D3D12_Resource, rhi::Object(m_swapChainBuffers[i]), textureDesc);
    }

    return true;
}

bool SwapChainD3D12::CreateFenceEvent()
{
    DXGI_SWAP_CHAIN_DESC swapChainDesc = GetSwapChainDesc();
    for (uint32_t bufferIndex = 0; bufferIndex < swapChainDesc.BufferCount; bufferIndex++)
    {
        m_frameFenceEvents.push_back(CreateEvent(nullptr, false, true, NULL));
    }
    return true;
}

void SwapChainD3D12::ReleaseRenderTargets(rhi::DevicePtr& device)
{
    device->WaitForIdle();
    device->RunGarbageCollection();

    for (auto e : m_frameFenceEvents)
    {
        SetEvent(e);
    }

    m_rhiSwapChainBuffers.clear();
    m_swapChainBuffers.clear();
}

uint32_t SwapChainD3D12::GetCurrentBackBufferIndex()
{
    return m_swapChain->GetCurrentBackBufferIndex();
}

uint32_t SwapChainD3D12::GetBackBufferCount()
{
    DXGI_SWAP_CHAIN_DESC swapChainDesc = GetSwapChainDesc();
    return swapChainDesc.BufferCount;
}

TexturePtr SwapChainD3D12::GetBackBuffer(uint32_t index)
{
    if (m_rhiSwapChainBuffers.size() > index)
    {
        return m_rhiSwapChainBuffers[index];
    }
    return nullptr;
}

core::StringView SwapChainD3D12::GetName()
{
    return m_name.c_str();
}

void SwapChainD3D12::Present(uint32_t syncInterval, core::TReferencePtr<ID3D12Fence>& fence, core::TReferencePtr<ID3D12CommandQueue>& graphicsQueue)
{
    DXGI_SWAP_CHAIN_DESC swapChainDesc = GetSwapChainDesc();
    uint32_t bufferIndex = m_swapChain->GetCurrentBackBufferIndex();
    uint32_t presentFlags = 0;
    if ((swapChainDesc.Flags & DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING) != 0)
    {
        presentFlags |= DXGI_PRESENT_ALLOW_TEARING;
    }

    m_swapChain->Present(syncInterval, presentFlags);

    fence->SetEventOnCompletion(m_frameCount, m_frameFenceEvents[bufferIndex]);
    graphicsQueue->Signal(fence, m_frameCount);
    m_frameCount++;
}

DXGI_SWAP_CHAIN_DESC SwapChainD3D12::GetSwapChainDesc()
{
    DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
    m_swapChain->GetDesc(&swapChainDesc);
    return swapChainDesc;
}

DXGI_SWAP_CHAIN_FULLSCREEN_DESC SwapChainD3D12::GetFullScreenDesc()
{
    DXGI_SWAP_CHAIN_FULLSCREEN_DESC fullScreenDesc = {};
    m_swapChain->GetFullscreenDesc(&fullScreenDesc);
    return fullScreenDesc;
}


class ContextD3D12 final : public IContext
{
public:
    bool Create(const SetupParam& param) override;
    bool Destroy() override;

    rhi::DevicePtr GetDevice() override;

    void AddWindow(WindowParam& param) override;
    void RemoveWindow(core::StringView name) override;
    bool HasWindow(core::StringView name) override;
    size_t GetWindowCount() override;

    bool Resize(core::StringView windowName, uint32_t width, uint32_t height) override;

    TexturePtr GetCurrentBackBuffer(core::StringView windowName) override;
    TexturePtr GetBackBuffer(core::StringView windowName, uint32_t index) override;
    uint32_t GetCurrentBackBufferIndex(core::StringView windowName) override;
    uint32_t GetBackBufferCount(core::StringView windowName) override;

    void Present(core::StringView windowName, uint32_t syncInterval) override;
    void PresentAll(uint32_t syncInterval) override;

    core::StringView GetAdapterName() override;
    GPUType GetGPUType() override;

private:
    bool CreateQueue();
    core::TReferencePtr<IDXGIAdapter> FindAdapter(core::TFunctionObject<void* ()> adapterFinder = nullptr);
    GPUType GetGPUType(DXGI_ADAPTER_DESC& desc);

private:
    SetupParam m_param = {};
    core::TReferencePtr<ID3D12Device> m_device = nullptr;
    core::TReferencePtr<ID3D12CommandQueue> m_graphicsQueue = nullptr;
    core::TReferencePtr<ID3D12CommandQueue> m_computeQueue = nullptr;
    core::TReferencePtr<ID3D12CommandQueue> m_copyQueue = nullptr;
    core::TReferencePtr<IDXGIAdapter> m_adapter = nullptr;
    core::TUnorderedMap<core::String, SwapChainD3D12Ptr> m_swapChains = {};
    core::TReferencePtr<ID3D12Fence> m_frameFence = nullptr;
    rhi::DevicePtr m_rhiDevice = nullptr;
    core::String m_adapterName = "";
    GPUType m_gpuType = GPUType::Nvidia;
};

bool ContextD3D12::Create(const SetupParam& param)
{
    m_param = param;
    m_adapter = FindAdapter();
    if (!m_adapter)
    {
        Destroy();
        return false;
    }

    DXGI_ADAPTER_DESC adapterDesc = {};
    m_adapter->GetDesc(&adapterDesc);
    m_adapterName = core::StringUtility::GetString(adapterDesc.Description);
    m_gpuType = GetGPUType(adapterDesc);

    HRESULT hr = D3D12CreateDevice(m_adapter, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_device));
    if (FAILED(hr))
    {
        Destroy();
        return false;
    }

    if (m_param.enableDebug)
    {
        core::TReferencePtr<ID3D12Debug> p_debug = nullptr;
        hr = D3D12GetDebugInterface(IID_PPV_ARGS(&p_debug));
        if (SUCCEEDED(hr))
        {
            p_debug->EnableDebugLayer();
        }

        core::TReferencePtr<ID3D12InfoQueue> pInfoQueue = nullptr;
        m_device->QueryInterface(&pInfoQueue);

        if (pInfoQueue)
        {
            pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
            pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
            D3D12_MESSAGE_ID disableMessageIDs[] =
            {
                D3D12_MESSAGE_ID_CLEARDEPTHSTENCILVIEW_MISMATCHINGCLEARVALUE,
                D3D12_MESSAGE_ID_COMMAND_LIST_STATIC_DESCRIPTOR_RESOURCE_DIMENSION_MISMATCH,
            };

            D3D12_INFO_QUEUE_FILTER filter = {};
            filter.DenyList.pIDList = disableMessageIDs;
            filter.DenyList.NumIDs = sizeof(disableMessageIDs) / sizeof(disableMessageIDs[0]);
            pInfoQueue->AddStorageFilterEntries(&filter);
        }
    }

    if (!CreateQueue())
    {
        Destroy();
        return false;
    }

    rhi::d3d12::DeviceDesc deviceDesc = {};
    deviceDesc.pDevice = m_device;
    deviceDesc.pGraphicsCommandQueue = m_graphicsQueue;
    deviceDesc.pComputeCommandQueue = m_computeQueue;
    deviceDesc.pCopyCommandQueue = m_copyQueue;
    m_rhiDevice = rhi::d3d12::Device::Create(deviceDesc);

    hr = m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_frameFence));
    if (FAILED(hr))
    {
        Destroy();
        return false;
    }

    return true;
}

bool ContextD3D12::Destroy()
{
    return true;
}

rhi::DevicePtr ContextD3D12::GetDevice()
{
    return m_rhiDevice;
}

void ContextD3D12::AddWindow(WindowParam& param)
{
    if (HasWindow(param.name))
    {
        return;
    }

    SwapChainD3D12* swapChain = SwapChainD3D12::Create(param, m_graphicsQueue, m_param.enableDebug);
    if (!swapChain)
    {
        SLUG_ASSERT_MSG(false, "Failed Create Swapchain");
        swapChain->Release();
        return;
    }

    if (!swapChain->CreateRenderTargets(m_rhiDevice))
    {
        SLUG_ASSERT_MSG(false, "Failed Create Swapchain Render Target");
        swapChain->Release();
        return;
    }

    if (!swapChain->CreateFenceEvent())
    {
        SLUG_ASSERT_MSG(false, "Failed Create FenceEvent");
        swapChain->Release();
        return;
    }


    m_swapChains[core::String(param.name)] = std::move(swapChain);
}

void ContextD3D12::RemoveWindow(core::StringView name)
{
    auto itr = m_swapChains.find(core::String(name));
    if (itr != m_swapChains.end())
    {
        m_swapChains.erase(itr);
    }
}

bool ContextD3D12::HasWindow(core::StringView name)
{
    auto itr = m_swapChains.find(core::String(name));
    if (itr != m_swapChains.end())
    {
        return true;
    }
    return false;
}

size_t ContextD3D12::GetWindowCount()
{
    return m_swapChains.size();
}

bool ContextD3D12::Resize(core::StringView windowName, uint32_t width, uint32_t height)
{
    if (HasWindow(windowName))
    {
        return m_swapChains[windowName.data()]->Resize(m_rhiDevice, width, height);
    }
    return false;
}

TexturePtr ContextD3D12::GetCurrentBackBuffer(core::StringView windowName)
{
    if (HasWindow(windowName))
    {
        uint32_t index = m_swapChains[windowName.data()]->GetCurrentBackBufferIndex();
        return GetBackBuffer(windowName, index);
    }
    return nullptr;
}

TexturePtr ContextD3D12::GetBackBuffer(core::StringView windowName, uint32_t index)
{
    if (HasWindow(windowName))
    {
        return m_swapChains[windowName.data()]->GetBackBuffer(index);
    }
    return nullptr;
}


uint32_t ContextD3D12::GetCurrentBackBufferIndex(core::StringView windowName)
{
    if (HasWindow(windowName))
    {
        m_swapChains[windowName.data()]->GetCurrentBackBufferIndex();
    }
    return 0;
}
uint32_t ContextD3D12::GetBackBufferCount(core::StringView windowName)
{
    if (HasWindow(windowName))
    {
        m_swapChains[windowName.data()]->GetBackBufferCount();
    }
    return 0;
}

void ContextD3D12::Present(core::StringView windowName, uint32_t syncInterval)
{
    if (HasWindow(windowName))
    {
        m_swapChains[windowName.data()]->Present(syncInterval, m_frameFence, m_graphicsQueue);
    }
}

void ContextD3D12::PresentAll(uint32_t syncInterval)
{
    for(auto& swapChain : m_swapChains)
    {
        swapChain.second->Present(syncInterval, m_frameFence, m_graphicsQueue);
    }
}

core::StringView ContextD3D12::GetAdapterName()
{
    return m_adapterName;
}

GPUType ContextD3D12::GetGPUType()
{
    return m_gpuType;
}

bool ContextD3D12::CreateQueue()
{
    HRESULT hr;
    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    ZeroMemory(&queueDesc, sizeof(queueDesc));

    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    queueDesc.NodeMask = 1;
    hr = m_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_graphicsQueue));
    if (FAILED(hr))
    {
        return false;
    }
    m_graphicsQueue->SetName(L"GraphicsQueue");

    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;
    hr = m_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_computeQueue));
    if (FAILED(hr))
    {
        return false;
    }
    m_computeQueue->SetName(L"ComputeQueue");

    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_COPY;
    hr = m_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_copyQueue));
    if (FAILED(hr))
    {
        return false;
    }
    m_copyQueue->SetName(L"CopyQueue");

    return true;
}

core::TReferencePtr<IDXGIAdapter> ContextD3D12::FindAdapter(core::TFunctionObject<void* ()> adapterFinder)
{
    core::TReferencePtr<IDXGIAdapter> adapter = {};
    if (adapterFinder)
    {
        adapter = static_cast<IDXGIAdapter*>(adapterFinder());
        return adapter;
    }

    core::TReferencePtr<IDXGIFactory1> dxgiFactory;
    HRESULT hres = CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory));
    if (hres != S_OK)
    {
        SLUG_ASSERT_MSG(false, "ERROR in CreateDXGIFactory.");
        return adapter;
    }

    uint32_t adapterIndex = 0;
    size_t maxMemory = 0;
    while (SUCCEEDED(hres))
    {
        core::TReferencePtr<IDXGIAdapter> pAdapter;
        hres = dxgiFactory->EnumAdapters(adapterIndex, &pAdapter);

        if (SUCCEEDED(hres))
        {
            DXGI_ADAPTER_DESC adapterDesc;
            pAdapter->GetDesc(&adapterDesc);

            if (maxMemory < adapterDesc.DedicatedVideoMemory)
            {
                maxMemory = adapterDesc.DedicatedVideoMemory;
                adapter = pAdapter;
            }
        }
        adapterIndex++;
    }
    return adapter;
}

GPUType ContextD3D12::GetGPUType(DXGI_ADAPTER_DESC& desc)
{
    if (desc.VendorId == 0x10DE)
    {
        return GPUType::Nvidia;
    }

    return GPUType::Nvidia;
}

core::TReferencePtr<IContext> ContextGenerator::GenerateD3D12(const IContext::SetupParam& param)
{
    core::TReferencePtr<IContext> device = new ContextD3D12();
    device->Create(param);
    return device;
}

}
}
