#pragma once

#include <d3d12.h>
#include "rhi/ResourceType.hpp"
#include "rhi/d3d12/interface/IDescriptorHeap.hpp"

namespace slug 
{
namespace rhi 
{
namespace d3d12 
{

struct DxgiFormatMapping
{
    enum UseType
    {
        Resource,
        SRV,
        RTV
    };

    FormatType  abstractFormat;
    DXGI_FORMAT resourceFormat;
    DXGI_FORMAT srvFormat;
    DXGI_FORMAT rtvFormat;
};

struct ShaderVisibilityMapping
{
    ShaderType shaderType;
    D3D12_SHADER_VISIBILITY shaderVisibility;
};

struct BlendMapping
{
    BlendFactor blendFactor;
    D3D12_BLEND d3d12Blend;
};

struct BlendOpMapping
{
    BlendOp blendOp;
    D3D12_BLEND_OP d3d12BlendOp;
};

struct StencilOpMapping
{
    StencilOp stencilOp;
    D3D12_STENCIL_OP d3d12StencilOp;
};

struct ComparisonFuncMapping
{
    ComparisonFunc comparisonFunc;
    D3D12_COMPARISON_FUNC d3d12ComparisonFunc;
};

struct PrimitiveTypeMapping
{
    PrimitiveType primitiveType;
    D3D_PRIMITIVE_TOPOLOGY d3dPrimitiveType;
};

struct D3D12PrimitiveTypologyTypeMapping
{
    PrimitiveType primitiveType;
    D3D12_PRIMITIVE_TOPOLOGY_TYPE d3d12PrimitiveType;
};

struct TextureAddressModeMapping
{
    SamplerAddressMode samplerAddressMode;
    D3D12_TEXTURE_ADDRESS_MODE d3d12TextureAddressMode;
};

struct SamplerReductionTypeMapping
{
    SamplerReductionType samplerReductionType;
    D3D12_FILTER_REDUCTION_TYPE d3d12FilterReductionType;
};

struct VariableShadingRateMapping
{
    VariableShadingRate variableShadingRate;
    D3D12_SHADING_RATE d3d12ShadingRate;
};

struct ShadingRateCombinerMapping
{
    ShadingRateCombiner shadingRateCombiner;
    D3D12_SHADING_RATE_COMBINER d3d12ShadingRateCombiner;
};

struct DX12ViewportState
{
    uint32_t numViewports = 0;
    D3D12_VIEWPORT viewports[16] = {};
    uint32_t numScissorRects = 0;
    D3D12_RECT scissorRects[16] = {};
};

class D3D12Utility
{
public:
    static const DxgiFormatMapping& GetDxgiFormatMapping(FormatType abstract_format);
    static const DxgiFormatMapping& GetDxgiFormatMapping(DXGI_FORMAT dxgiFormat, DxgiFormatMapping::UseType useType);
    static const ShaderVisibilityMapping& GetShaderVisibilityMapping(ShaderType shaderType);
    static const ShaderVisibilityMapping& GetShaderVisibilityMapping(D3D12_SHADER_VISIBILITY shaderVisibility);
    static const BlendMapping& GetBlendMapping(BlendFactor factor);
    static const BlendMapping& GetBlendMapping(D3D12_BLEND blend);
    static const BlendOpMapping& GetBlendOpMapping(BlendOp blendOp);
    static const BlendOpMapping& GetBlendOpMapping(D3D12_BLEND_OP blendOp);
    static const StencilOpMapping& GetStencilOpMapping(StencilOp stencilOp);
    static const StencilOpMapping& GetStencilOpMapping(D3D12_STENCIL_OP stencilOp);
    static const ComparisonFuncMapping& GetComparisonFuncMapping(ComparisonFunc comparisonFunc);
    static const ComparisonFuncMapping& GetComparisonFuncMapping(D3D12_COMPARISON_FUNC d3d12ComparisonFunc);
    static const PrimitiveTypeMapping& GetPrimitiveTypeMapping(PrimitiveType primitiveType);
    static const PrimitiveTypeMapping& GetPrimitiveTypeMapping(D3D_PRIMITIVE_TOPOLOGY d3dPrimitiveType);
    static const D3D12PrimitiveTypologyTypeMapping& GetD3D12PrimitiveTypeMapping(PrimitiveType primitiveType);
    static const D3D12PrimitiveTypologyTypeMapping& GetD3D12PrimitiveTypeMapping(D3D12_PRIMITIVE_TOPOLOGY_TYPE d3d12PrimitiveType);
    static const TextureAddressModeMapping& GetTextureAddressModeMapping(SamplerAddressMode samplerAddressMode);
    static const TextureAddressModeMapping& GetTextureAddressModeMapping(D3D12_TEXTURE_ADDRESS_MODE d3d12TextureAddressMode);
    static const SamplerReductionTypeMapping& GetSamplerReductionTypeMapping(SamplerReductionType samplerReductionType);
    static const SamplerReductionTypeMapping& GetSamplerReductionTypeMapping(D3D12_FILTER_REDUCTION_TYPE d3d12FilterReductionType);
    static const VariableShadingRateMapping& GetVariableShadingRateMapping(VariableShadingRate variableShadingRate);
    static const VariableShadingRateMapping& GetVariableShadingRateMapping(D3D12_SHADING_RATE d3d12ShadingRate);
    static const ShadingRateCombinerMapping& GetShadingRateCombinerMapping(ShadingRateCombiner shadingRateCombiner);
    static const ShadingRateCombinerMapping& GetShadingRateCombinerMapping(D3D12_SHADING_RATE_COMBINER d3d12ShadingRateCombiner);
public:

    static DXGI_FORMAT ConvertFormat(FormatType formatType);
    static FormatType ConvertFormat(DXGI_FORMAT formatType, DxgiFormatMapping::UseType useType);

    static D3D12_SHADER_VISIBILITY ConvertShaderStage(ShaderType shaderType);
    static ShaderType ConvertShaderStage(D3D12_SHADER_VISIBILITY shaderVisibility);

    static D3D12_BLEND ConvertBlendValue(BlendFactor value);
    static BlendFactor ConvertBlendValue(D3D12_BLEND value);

    static D3D12_BLEND_OP ConvertBlendOp(BlendOp value);
    static BlendOp ConvertBlendOp(D3D12_BLEND_OP value);

    static D3D12_STENCIL_OP ConvertStencilOp(rhi::StencilOp value);
    static rhi::StencilOp ConvertStencilOp(D3D12_STENCIL_OP value);

    static D3D12_COMPARISON_FUNC ConvertComparisonFunc(rhi::ComparisonFunc value);
    static rhi::ComparisonFunc ConvertComparisonFunc(D3D12_COMPARISON_FUNC value);

    static D3D_PRIMITIVE_TOPOLOGY ConvertPrimitiveType(rhi::PrimitiveType value, uint32_t controlPoints);
    static rhi::PrimitiveType ConvertPrimitiveType(D3D12_PRIMITIVE_TOPOLOGY value, uint32_t& controlPoints);

    static D3D12_PRIMITIVE_TOPOLOGY_TYPE ConvertD3D12PrimitiveType(rhi::PrimitiveType value);
    static rhi::PrimitiveType ConvertD3D12PrimitiveType(D3D12_PRIMITIVE_TOPOLOGY_TYPE value);

    static D3D12_TEXTURE_ADDRESS_MODE ConvertSamplerAddressMode(rhi::SamplerAddressMode value);
    static rhi::SamplerAddressMode ConvertSamplerAddressMode(D3D12_TEXTURE_ADDRESS_MODE value);

    static UINT ConvertSamplerReductionType(rhi::SamplerReductionType value);
    static rhi::SamplerReductionType ConvertSamplerReductionType(UINT value);

    static D3D12_SHADING_RATE ConvertPixelShadingRate(rhi::VariableShadingRate value);
    static rhi::VariableShadingRate ConvertPixelShadingRate(D3D12_SHADING_RATE value);

    static D3D12_SHADING_RATE_COMBINER ConvertShadingRateCombiner(rhi::ShadingRateCombiner value);
    static rhi::ShadingRateCombiner ConvertShadingRateCombiner(D3D12_SHADING_RATE_COMBINER value);
public:
    static void TranslateBlendState(const rhi::BlendState& inState, D3D12_BLEND_DESC& outState);
    static void TranslateBlendState(const D3D12_BLEND_DESC& inState, rhi::BlendState& outState);

    static void TranslateDepthStencilState(const rhi::DepthStencilState& inState, D3D12_DEPTH_STENCIL_DESC& outState);
    static void TranslateDepthStencilState(const D3D12_DEPTH_STENCIL_DESC& inState, rhi::DepthStencilState& outState);

    static void TranslateRasterizerState(const rhi::RasterState& inState, D3D12_RASTERIZER_DESC& outState);
    static void TranslateRasterizerState(const D3D12_RASTERIZER_DESC& inState, rhi::RasterState& outState);

    static D3D12_RESOURCE_DESC ConvertTextureDesc(const TextureDesc& d);

    static D3D12_CLEAR_VALUE ConvertTextureClearValue(const TextureDesc& d);

    static D3D12_RESOURCE_STATES ConvertResourceStates(ResourceStatesBitset stateBits);

    static D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAGS ConvertAcceslerationStructureBuildFlags(const rt::AccelStructBuildFlagsBitset& buildFlags);

    static DX12ViewportState ConvertViewportState(const RasterState& rasterState, const FrameBufferInfoEx& framebufferInfo, const ViewportState& vpState);
public:
    static void WaitForFence(ID3D12Fence* fence, uint64_t value, HANDLE event);

    static bool IsBlendFactorRequired(rhi::BlendFactor value);
    
    static uint32_t CalcSubresource(uint32_t mip_slice, uint32_t array_slice, uint32_t plane_slice, uint32_t mip_levels, uint32_t array_size);
};
}
}
}
