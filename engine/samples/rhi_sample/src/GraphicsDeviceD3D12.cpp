#include "core/Function.hpp"
#include "render/IGrapchicsDevice.hpp"
#include "rhi/Rhi.hpp"
#include "rhi/d3d12/D3D12.hpp"
#include "rhi/d3d12/resource/Device.hpp"
#include "rhi/common/Resource.hpp"
#include <dxgi1_5.h>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib ")

namespace slug::render
{

class GraphicsSwapChainD3D12
{
public:
    bool Create(uint32_t width, uint32_t height, core::TReferencePtr<ID3D12CommandQueue>& graphicsQueue, bool enableDebug);
    bool Resize(rhi::DevicePtr& device, uint32_t width, uint32_t height);
    bool CheckAllowTearingSupport(core::TReferencePtr<IDXGIFactory2>& factory);
    bool CreateRenderTargets(rhi::DevicePtr& device);
    bool CreateFenceEvent();
    void ReleaseRenderTargets(rhi::DevicePtr& device);
    void Present(uint32_t syncInterval, core::TReferencePtr<ID3D12Fence>& fence, core::TReferencePtr<ID3D12CommandQueue>& graphicsQueue);
    uint32_t GetCurrentBackBufferIndex();
    uint32_t GetBackBufferCount();

private:
    DXGI_SWAP_CHAIN_DESC GetSwapChainDesc();
    DXGI_SWAP_CHAIN_FULLSCREEN_DESC GetFullScreenDesc();

private:
    core::TReferencePtr<IDXGISwapChain3> m_swapChain = nullptr;
    core::TVector<core::TReferencePtr<ID3D12Resource>> m_swapChainBuffers = {};
    core::TVector<rhi::TexturePtr> m_rhiSwapChainBuffers = {};
    core::TVector<HANDLE> m_frameFenceEvents = {};
    uint32_t m_frameCount = 0;
    bool m_tearingSupport = true;
    bool m_vcync = true;
    HWND m_windowHandle = {};
};

bool GraphicsSwapChainD3D12::Create(uint32_t width, uint32_t height, core::TReferencePtr<ID3D12CommandQueue>& graphicsQueue, bool enableDebug)
{
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
    ZeroMemory(&swapChainDesc, sizeof(m_swapChain));
    swapChainDesc.Width = width;
    swapChainDesc.Height = height;
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
        return false;
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
    hr = pDxgiFactory->CreateSwapChainForHwnd(graphicsQueue, m_windowHandle, &swapChainDesc, &fullscreenDesc, nullptr, &pSwapChain1);
    if (FAILED(hr))
    {
        return false;
    }

    hr = pSwapChain1->QueryInterface(IID_PPV_ARGS(&m_swapChain));
    if (FAILED(hr))
    {
        return false;
    }
    return true;
}

bool GraphicsSwapChainD3D12::Resize(rhi::DevicePtr& device, uint32_t width, uint32_t height)
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

bool GraphicsSwapChainD3D12::CheckAllowTearingSupport(core::TReferencePtr<IDXGIFactory2>& factory)
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

bool GraphicsSwapChainD3D12::CreateRenderTargets(rhi::DevicePtr& device)
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

bool GraphicsSwapChainD3D12::CreateFenceEvent()
{
    DXGI_SWAP_CHAIN_DESC swapChainDesc = GetSwapChainDesc();
    for (uint32_t bufferIndex = 0; bufferIndex < swapChainDesc.BufferCount; bufferIndex++)
    {
        m_frameFenceEvents.push_back(CreateEvent(nullptr, false, true, NULL));
    }
    return true;
}

void GraphicsSwapChainD3D12::ReleaseRenderTargets(rhi::DevicePtr& device)
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

uint32_t GraphicsSwapChainD3D12::GetCurrentBackBufferIndex()
{
    return m_swapChain->GetCurrentBackBufferIndex();
}

uint32_t GraphicsSwapChainD3D12::GetBackBufferCount()
{
    DXGI_SWAP_CHAIN_DESC swapChainDesc = GetSwapChainDesc();
    return swapChainDesc.BufferCount;
}

void GraphicsSwapChainD3D12::Present(uint32_t syncInterval, core::TReferencePtr<ID3D12Fence>& fence, core::TReferencePtr<ID3D12CommandQueue>& graphicsQueue)
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

DXGI_SWAP_CHAIN_DESC GraphicsSwapChainD3D12::GetSwapChainDesc()
{
    DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
    m_swapChain->GetDesc(&swapChainDesc);
    return swapChainDesc;
}

DXGI_SWAP_CHAIN_FULLSCREEN_DESC GraphicsSwapChainD3D12::GetFullScreenDesc()
{
    DXGI_SWAP_CHAIN_FULLSCREEN_DESC fullScreenDesc = {};
    m_swapChain->GetFullscreenDesc(&fullScreenDesc);
    return fullScreenDesc;
}


class GraphicsDeviceD3D12 final : public IGraphicsDevice
{
public:
    bool Create(const GraphicsDeviceParameter& param) override;
    bool Destroy() override;
    bool Resize(uint32_t width, uint32_t height) override;
    bool GetCurrentBackBuffer() override;
    uint32_t GetCurrentBackBufferIndex() override;
    uint32_t GetBackBufferCount() override;
    void Present(uint32_t syncInterval) override;
    const char* GetAdapterName() override;
    GPUType GetGPUType() override;

private:
    bool CreateQueue();
    core::TReferencePtr<IDXGIAdapter> FindAdapter(core::TFunctionObject<void*()> adapterFinder = nullptr);
    GPUType GetGPUType(DXGI_ADAPTER_DESC& desc);
    void AdjustWindowOnAdapter(core::TReferencePtr<IDXGIAdapter>& adapter, const core::TReferencePtr<IWindow>& window);

private:
    GraphicsDeviceParameter m_param = {};
    core::TReferencePtr<ID3D12Device> m_device = nullptr;
    core::TReferencePtr<ID3D12CommandQueue> m_graphicsQueue = nullptr;
    core::TReferencePtr<ID3D12CommandQueue> m_computeQueue = nullptr;
    core::TReferencePtr<ID3D12CommandQueue> m_copyQueue = nullptr;
    core::TReferencePtr<IDXGIAdapter> m_adapter = nullptr;
    GraphicsSwapChainD3D12 m_swapChain = {};
    core::TReferencePtr<ID3D12Fence> m_frameFence = nullptr;
    rhi::DevicePtr m_rhiDevice = nullptr;
    core::String m_adapterName = "";
    GPUType m_gpuType = GPUType::Nvidia;
};

bool GraphicsDeviceD3D12::Create(const GraphicsDeviceParameter& param)
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
    AdjustWindowOnAdapter(m_adapter, m_param.window);

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

    if (!m_swapChain.Create(m_param.width, m_param.height, m_graphicsQueue, m_param.enableDebug))
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

    if (m_swapChain.CreateRenderTargets(m_rhiDevice))
    {
        Destroy();
        return false;
    }

    hr = m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_frameFence));
    if (FAILED(hr))
    {
        Destroy();
        return false;
    }

    return true;
}

bool GraphicsDeviceD3D12::CreateQueue()
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

bool GraphicsDeviceD3D12::Destroy()
{
    return true;
}

bool GraphicsDeviceD3D12::Resize(uint32_t width, uint32_t height)
{
    return m_swapChain.Resize(m_rhiDevice, width, height);
}

bool GraphicsDeviceD3D12::GetCurrentBackBuffer()
{
    return false;
}

uint32_t GraphicsDeviceD3D12::GetCurrentBackBufferIndex()
{
    return m_swapChain.GetCurrentBackBufferIndex();
}

uint32_t GraphicsDeviceD3D12::GetBackBufferCount()
{
    return m_swapChain.GetBackBufferCount();
}

void GraphicsDeviceD3D12::Present(uint32_t syncInterval)
{
    m_swapChain.Present(syncInterval, m_frameFence, m_graphicsQueue);
}

const char* GraphicsDeviceD3D12::GetAdapterName()
{
    return m_adapterName.c_str();
}

GPUType GraphicsDeviceD3D12::GetGPUType()
{
    return m_gpuType;
}

core::TReferencePtr<IDXGIAdapter> GraphicsDeviceD3D12::FindAdapter(core::TFunctionObject<void*()> adapterFinder)
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

            if (maxMemory < adapterDesc.DedicatedSystemMemory)
            {
                maxMemory = adapterDesc.DedicatedVideoMemory;
                adapter = pAdapter;
            }
        }
        adapterIndex++;
    }
    return adapter;
}

GPUType GraphicsDeviceD3D12::GetGPUType(DXGI_ADAPTER_DESC& desc)
{
    if (desc.VendorId == 0x10DE)
    {
        return GPUType::Nvidia;
    }

    return GPUType::Nvidia;
}

void GraphicsDeviceD3D12::AdjustWindowOnAdapter(core::TReferencePtr<IDXGIAdapter>& adapter, const core::TReferencePtr<IWindow>& window)
{
    if (!adapter)
    {
        return;
    }

    HRESULT hres = S_OK;
    uint32_t outputIndex = 0;
    while (SUCCEEDED(hres))
    {
        IDXGIOutput* pOutput = nullptr;
        hres = adapter->EnumOutputs(outputIndex++, &pOutput);

        if (SUCCEEDED(hres) && pOutput)
        {
            DXGI_OUTPUT_DESC OutputDesc;
            pOutput->GetDesc(&OutputDesc);
#if 0
            const RECT desktop = OutputDesc.DesktopCoordinates;
            const RECT desktop = OutputDesc.DesktopCoordinates;
            const int32_t centreX = (int)desktop.left + (int)(desktop.right - desktop.left) / 2;
            const int32_t centreY = (int)desktop.top + (int)(desktop.bottom - desktop.top) / 2;
            const int32_t winW = rect.right - rect.left;
            const int32_t winH = rect.bottom - rect.top;
            const int32_t left = centreX - winW / 2;
            const int32_t right = left + winW;
            const int32_t top = centreY - winH / 2;
            const int32_t bottom = top + winH;
#endif
            return;
        }
    }
}

core::TReferencePtr<IGraphicsDevice> GraphicsDeviceGenerator::GenerateD3D12(GraphicsDeviceParameter param)
{
    core::TReferencePtr<IGraphicsDevice> device = core::MakeReference<GraphicsDeviceD3D12>();
    device->Create(param);
    return device;
}

}
