#pragma once

#include "core/TypeConcept.hpp"
#include "core/NumericLimits.hpp"
#include "core/container/EnumBitset.hpp"
#include "core/container/UnorderedMap.hpp"
#include "core/container/StaticVector.hpp"
#include "core/container/String.hpp"
#include "core/memory/ReferencePtr.hpp"
#include "core/color/Color.hpp"

namespace slug::rhi
{
static constexpr uint32_t MAX_RENDER_TARGETS = 8;
static constexpr uint32_t MAX_VIEWPORTS = 16;
static constexpr uint32_t MAX_VERTEX_ATTRIBUTES = 16;
static constexpr uint32_t MAX_BINDING_LAYOUTS = 5;
static constexpr uint32_t MAX_BINDING_PER_LAYOUT = 128;
static constexpr uint32_t MAX_VOLATILE_CONSTANT_BUFFERS_PER_LAYOUT = 6;
static constexpr uint32_t MAX_VOLATILE_CONSTANT_BUFFERS = 32;
static constexpr uint32_t MAX_PUSH_CONSTANT_SIZE = 128;
static constexpr uint32_t CONSTANT_BUFFER_OFFSET_SIZE_ALINMENT = 256;

using IResource = core::ReferenceObject;
using Object = core::InternalObject;
using ObjectType = core::ObjectType;
using Color = core::Color;

template <class T>
void HashCombine(size_t& seed, const T& v)
{
    core::THash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

struct Viewport 
{
    float min_x, max_x;
    float min_y, max_y;
    float min_z, max_z;

    Viewport()
        : min_x(0.0f), max_x(0.0f)
        , min_y(0.0f), max_y(0.0f)
        , min_z(0.0f), max_z(0.0f)
    {}

    Viewport(float width, float height)
        : min_x(0.0f), max_x(width)
        , min_y(0.0f), max_y(height)
        , min_z(0.0f), max_z(1.0f)
    {}

    Viewport(float _min_x, float _max_x, float _min_y, float _max_y, float _min_z, float _max_z)
        : min_x(_min_x), max_x(_max_x)
        , min_y(_min_y), max_y(_max_y)
        , min_z(_min_z), max_z(_max_z)
    {}

    bool operator ==(const Viewport& b) const
    {
        return min_x == b.min_x
            && min_y == b.min_y
            && min_z == b.min_z
            && max_x == b.max_x
            && max_y == b.max_y
            && max_z == b.max_z;
    }

    bool operator !=(const Viewport& b) const
    {
        return !(*this == b);
    }

    float width() const
    {
        return max_x - min_x;
    }
    
    float height() const
    {
        return max_y - min_y;
    }
};

struct Rect {
    int32_t min_x, max_x;
    int32_t min_y, max_y;

    Rect() : min_x(0), max_x(0), min_y(0), max_y(0)
    {}
    Rect(int32_t width, int32_t height) : min_x(0), max_x(width), min_y(0), max_y(height)
    {}
    Rect(int32_t _min_x, int32_t _max_x, int32_t _min_y, int32_t _max_y) : min_x(_min_x), max_x(_max_x), min_y(_min_y), max_y(_max_y)
    {}
    explicit Rect(const Viewport& viewport)
        : min_x(int32_t(floorf(viewport.min_x)))
        , max_x(int32_t(ceilf(viewport.max_x)))
        , min_y(int32_t(floorf(viewport.min_y)))
        , max_y(int32_t(ceilf(viewport.max_y)))
    {}

    bool operator ==(const Rect& b) const
    {
        return min_x == b.min_x && min_y == b.min_y && max_x == b.max_x && max_y == b.max_y;
    }
    bool operator !=(const Rect& b) const
    {
        return !(*this == b);
    }

    int32_t width() const
    {
        return max_x - min_x;
    }

    int32_t height() const
    {
        return max_y - min_y;
    }
};

enum class GraphicsApiType : uint8_t 
{
    D3D11,
    D3D12,
    VULKAN
};

enum class FormatType : uint8_t 
{
    UNKNOWN,

    R8_UINT,
    R8_SINT,
    R8_UNORM,
    R8_SNORM,
    RG8_UINT,
    RG8_SINT,
    RG8_UNORM,
    RG8_SNORM,
    R16_UINT,
    R16_SINT,
    R16_UNORM,
    R16_SNORM,
    R16_FLOAT,
    BGRA4_UNORM,
    B5G6R5_UNORM,
    B5G5R5A1_UNORM,
    RGBA8_UINT,
    RGBA8_SINT,
    RGBA8_UNORM,
    RGBA8_SNORM,
    BGRA8_UNORM,
    SRGBA8_UNORM,
    SBGRA8_UNORM,
    R10G10B10A2_UNORM,
    R11G11B10_FLOAT,
    RG16_UINT,
    RG16_SINT,
    RG16_UNORM,
    RG16_SNORM,
    RG16_FLOAT,
    R32_UINT,
    R32_SINT,
    R32_FLOAT,
    RGBA16_UINT,
    RGBA16_SINT,
    RGBA16_FLOAT,
    RGBA16_UNORM,
    RGBA16_SNORM,
    RG32_UINT,
    RG32_SINT,
    RG32_FLOAT,
    RGB32_UINT,
    RGB32_SINT,
    RGB32_FLOAT,
    RGBA32_UINT,
    RGBA32_SINT,
    RGBA32_FLOAT,

    D16,
    D24S8,
    X24G8_UINT,
    D32,
    D32S8,
    X32G8_UINT,

    BC1_UNORM,
    BC1_UNORM_SRGB,
    BC2_UNORM,
    BC2_UNORM_SRGB,
    BC3_UNORM,
    BC3_UNORM_SRGB,
    BC4_UNORM,
    BC4_SNORM,
    BC5_UNORM,
    BC5_SNORM,
    BC6H_UFLOAT,
    BC6H_SFLOAT,
    BC7_UNORM,
    BC7_UNORM_SRGB,

    COUNT,
};

enum class FormatKind : uint8_t 
{
    Integer,
    Normalized,
    Float,
    DepthStencil
};

struct FormatInfo 
{
    FormatType format;
    const char* name;
    uint8_t bytesPerBlock;
    uint8_t blockSize;
    FormatKind kind;
    bool hasRed : 1;
    bool hasGreen : 1;
    bool hasBlur : 1;
    bool hasAlpha : 1;
    bool hasDepth : 1;
    bool hasStencil : 1;
    bool isSigned : 1;
    bool isSrgb : 1;
};

const FormatInfo& GetFormatInfo(FormatType format);

enum class FormatSupport : uint32_t 
{
    None = 0,
    Buffer,
    IndexBuffer,
    VertexBuffer,
    Texture,
    DepthStencil,
    RenderTarget,
    Blendable,
    ShaderLoad,
    ShaderSample,
    ShaderUavLoad,
    ShaderUavStore,
    ShaderAtomic,
    Count
};
using FormatSupportBitset = core::TEnumBitset<FormatSupport>;

enum class HeapType : uint8_t 
{
    DeviceLocal,
    Upload,
    ReadBack
};

enum class TextureDimension : uint8_t 
{
    Unknown,
    Texture1D,
    Texture1DArray,
    Texture2D,
    Texture2DArray,
    TextureCube,
    TextureCubeArray,
    Texture2DMS,
    Texture2DMSArray,
    Texture3D
};

enum class TextureType : uint8_t 
{
    RenderTarget,
    UAV,
    Typeless,
    StagingTexture,
    Count
};
using TextureTypeBitset = core::TEnumBitset<TextureType>();

enum class SharedResourceFlags : uint32_t 
{
    None = 0,
    Shared,
    Shared_NTHandle,
    Shared_CrossAdapter,
    Count
};
using SharedResourceFlagsBitset = core::TEnumBitset<SharedResourceFlags>;

enum class CpuAccessMode : uint8_t 
{
    None,
    Read,
    Write
};

enum class ResourceStates : uint32_t 
{
    Unknown = 0,
    Common,
    ConstantBuffer,
    VertexBuffer,
    IndexBuffer,
    IndirectArgument,
    ShaderResource,
    UnorderedAccess,
    RenderTarget,
    DepthWrite,
    DepthRead,
    StreamOut,
    CopyDest,
    CopySource,
    ResolveDest,
    ResolveSource,
    Present,
    AccelStructRead,
    AccelStructWrite,
    AccelStructBuildInput,
    AccelStructBuildBlas,
    ShadingRateSurface,
    OpacityMicromapWrite,
    OpacityMicromapBuildInput,
    Count
};
using ResourceStatesBitset = core::TEnumBitset<ResourceStates>;

using MipLevel = uint32_t;
using ArraySlice = uint32_t;

enum class ResourceType : uint8_t 
{
    None,
    Texture_SRV,
    Texture_UAV,
    TypedBuffer_SRV,
    TypedBuffer_UAV,
    StructuredBuffer_SRV,
    StructuredBuffer_UAV,
    RawBuffer_SRV,
    RawBuffer_UAV,
    ConstantBuffer,
    VolatileConstantBuffer,
    Sampler,
    RayTracingAccelStruct,
    PushConstants,
    Count
};

enum class ShaderType : uint16_t {
    None,
    Compute,
    Vertex,
    Hull,
    Domain,
    Geometry,
    Pixel,
    Amplification,
    Mesh,
    AllGraphics,
    RayGeneration,
    AnyHit,
    ClosestHit,
    Miss,
    Intersection,
    Callable,
    AllRayTracing,
    All,
    Count
};
using ShaderTypeBitset = core::TEnumBitset<ShaderType>;

enum class FastGeometryShaderFlags : uint8_t 
{
    ForceFastGS ,
    UseViewportMask,
    OffsetTargetIndexByViewportIndex,
    StrictApiOrder,
    Count
};
using FastGeometryShaderFlagsBitset = core::TEnumBitset<FastGeometryShaderFlags>;

enum class BlendFactor : uint8_t 
{
    Zero = 1,
    One = 2,
    SrcColor = 3,
    InvSrcColor = 4,
    SrcAlpha = 5,
    InvSrcAlpha = 6,
    DstAlpha = 7,
    InvDstAlpha = 8,
    DstColor = 9,
    InvDstColor = 10,
    SrcAlphaSaturate = 11,
    ConstantColor = 14,
    InvConstantColor = 15,
    Src1Color = 16,
    InvSrc1Color = 17,
    Src1Alpha = 18,
    InvSrc1Alpha = 19,
    OneMinusSrcColor = InvSrcColor,
    OneMinusSrcAlpha = InvSrcAlpha,
    OneMinusDstAlpha = InvDstAlpha,
    OneMinusDstColor = InvDstColor,
    OneMinusConstantColor = InvConstantColor,
    OneMinusSrc1Color = InvSrc1Color,
    OneMinusSrc1Alpha = InvSrc1Alpha,
};

enum class BlendOp : uint8_t 
{
    Add = 1,
    Subrtact = 2,
    ReverseSubtract = 3,
    Min = 4,
    Max = 5
};

enum class ColorMask : uint8_t 
{
    Red = 0,
    Green ,
    Blue,
    Alpha,
    Count
};
using ColorMaskBitset = core::TEnumBitset<ColorMask>;

enum class RasterFillMode : uint8_t 
{
    Solid,
    Wireframe,
    Fill = Solid,
    Line = Wireframe
};

enum class RasterCullMode : uint8_t 
{
    Back,
    Front,
    None
};

enum class StencilOp : uint8_t 
{
    Keep = 1,
    Zero = 2,
    Replace = 3,
    IncrementAndClamp = 4,
    DecrementAndClamp = 5,
    Invert = 6,
    IncrementAndWrap = 7,
    DecrementAndWrap = 8
};

enum class ComparisonFunc : uint8_t 
{
    Never = 1,
    Less = 2,
    Equal = 3,
    LessOrEqual = 4,
    Greater = 5,
    NotEqual = 6,
    GreaterOrEqual = 7,
    Always = 8
};

enum class SamplerAddressMode : uint8_t 
{
    Clamp,
    Wrap,
    Border,
    Mirror,
    MirrorOnce,
    ClampToEdge = Clamp,
    Repeat = Wrap,
    ClampToBorder = Border,
    MirroredRepeat = Mirror,
    MirrorClampToEdge = MirrorOnce
};

enum class SamplerReductionType : uint8_t 
{
    Standard,
    Comparison,
    Minimum,
    Maximum
};

enum class Feature : uint8_t 
{
    DeferredCommandLists,
    SinglePassStereo,
    RayTracingAccelStruct,
    RayTracingPipeline,
    RayTracingOpacityMicromap,
    RayQuery,
    ShaderExecutionReordering,
    FastGeometryShader,
    Meshlets,
    ConservativeRasterization,
    VariableRateShading,
    ShaderSpecializations,
    VirtualResources,
    ComputeQueue,
    CopyQueue,
    ConstantBufferRanges,
    Count
};
using FeatureBitset = core::TEnumBitset<Feature>;

enum class MessageSeverity : uint8_t 
{
    Info,
    Warning,
    Error,
    Fatal
};

enum class CommandQueue : uint8_t 
{
    Graphics = 0,
    Compute,
    Copy,

    Count
};

enum class VariableShadingRate : uint8_t 
{
    e1x1,
    e1x2,
    e2x1,
    e2x2,
    e2x4,
    e4x2,
    e4x4
};

enum class ShadingRateCombiner : uint8_t 
{
    Passthrough,
    Override,
    Min,
    Max,
    ApplyRelative
};

enum class PrimitiveType : uint8_t 
{
    PointList,
    LineList,
    TriangleList,
    TriangleStrip,
    TriangleFan,
    TriangleListWithAdjacency,
    TriangleStripWithAdjacency,
    PatchList,
    Undifine
};

struct MemoryRequirements 
{
    uint64_t size = 0;
    uint64_t alignment = 0;
};

struct BlendState
{
    struct RenderTarget
    {
        bool blendEnable = false;
        BlendFactor srcBlend = BlendFactor::One;
        BlendFactor destBlend = BlendFactor::Zero;
        BlendOp blendOp = BlendOp::Add;
        BlendFactor srcBlendAlpha = BlendFactor::One;
        BlendFactor destBlendAlpha = BlendFactor::Zero;
        BlendOp blendOpAlpha = BlendOp::Add;
        ColorMaskBitset colorWriteMask = {};

        RenderTarget()
        {
            colorWriteMask.Set();
        }

        constexpr bool operator ==(const RenderTarget& other) const
        {
            return blendEnable == other.blendEnable
                && srcBlend == other.srcBlend
                && destBlend == other.destBlend
                && blendOp == other.blendOp
                && srcBlendAlpha == other.srcBlendAlpha
                && destBlendAlpha == other.destBlendAlpha
                && blendOpAlpha == other.blendOpAlpha
                && colorWriteMask == other.colorWriteMask;
        }

        constexpr bool operator !=(const RenderTarget& other) const
        {
            return !(*this == other);
        }

        bool GetUsesConstantColor() const
        {
            return srcBlend == BlendFactor::ConstantColor
                || srcBlend == BlendFactor::OneMinusConstantColor
                || destBlend == BlendFactor::ConstantColor
                || destBlend == BlendFactor::OneMinusConstantColor
                || srcBlendAlpha == BlendFactor::ConstantColor
                || srcBlendAlpha == BlendFactor::OneMinusConstantColor
                || destBlendAlpha == BlendFactor::ConstantColor
                || destBlendAlpha == BlendFactor::OneMinusConstantColor;
        }
    };

    RenderTarget targets[MAX_RENDER_TARGETS];
    bool alphaToCoverageEnable = false;

    bool GetUsesConstantColor(uint32_t numTargets) const
    {
        for (uint32_t rt = 0; rt < numTargets; rt++) {
            if (targets[rt].GetUsesConstantColor())
            {
                return true;
            }
        }
        return false;
    }

    constexpr bool operator ==(const BlendState& other) const
    {
        if (alphaToCoverageEnable != other.alphaToCoverageEnable)
        {
            return false;
        }

        for (uint32_t i = 0; i < MAX_RENDER_TARGETS; ++i)
        {
            if (targets[i] != other.targets[i])
            {
                return false;
            }
        }

        return true;
    }

    constexpr bool operator !=(const BlendState& other) const
    {
        return !(*this == other);
    }
};

struct RasterState
{
    RasterFillMode fillMode = RasterFillMode::Solid;
    RasterCullMode cullMode = RasterCullMode::Back;
    bool frontCounterClockwise = false;
    bool depthClipEnable = false;
    bool scissorEnable = false;
    bool multisampleEnable = false;
    bool antialiasedLineEnable = false;
    int32_t depthBias = 0;
    float depthBiasClamp = 0.f;
    float slopeScaledDepthBias = 0.f;
    uint8_t forcedSampleCount = 0;
    bool programmableSamplePositionsEnable = false;
    bool conservativeRasterEnable = false;
    bool quadFillEnable = false;
    char samplePositionsX[16]{};
    char samplePositionsY[16]{};
};

struct DepthStencilState
{
    struct StencilOpDesc
    {
        StencilOp failOp = StencilOp::Keep;
        StencilOp depthFailOp = StencilOp::Keep;
        StencilOp passOp = StencilOp::Keep;
        ComparisonFunc stencilFunc = ComparisonFunc::Always;
    };

    bool depthTestEnable = true;
    bool depthWriteEnable = true;
    ComparisonFunc depthFunc = ComparisonFunc::Less;
    bool stencilEnable = false;
    uint8_t stencilReadMask = 0xff;
    uint8_t stencilWriteMask = 0xff;
    uint8_t stencilRefValue = 0;
    StencilOpDesc frontFaceStencil;
    StencilOpDesc backFaceStencil;
};

struct ViewportState
{
    core::TStaticVector<Viewport, MAX_VIEWPORTS> viewports;
    core::TStaticVector<Rect, MAX_VIEWPORTS> scissorRects;
};

namespace rt
{
enum class OpacityMicromapFormat 
{
    OC1_2_State = 0,
    OC1_4_State,
};

enum class OpacityMicromapBuildFlags : uint8_t 
{
    None = 0,
    FastTrace,
    FastBuild,
    Count
};
using OpacityMicromapBuildFlagsBitset = core::TEnumBitset<OpacityMicromapBuildFlags>;

enum class GeometryFlags : uint8_t 
{
    None = 0,
    Opaque,
    NoDuplicateAnyHitInvocation,
    Count
};
using GeometryFlagsBitset = core::TEnumBitset<GeometryFlags>;

enum class GeometryType : uint8_t 
{
    Triangles = 0,
    AABBs
};

enum class InstanceFlags : unsigned 
{
    None = 0,
    TriangleCullDisable,
    TriangleFrontCounterwise,
    ForceOpaque,
    ForceNonOpaque,
    ForceOMM2State,
    DisableOMMs,
    Count
};
using InstanceFlagsBitset = core::TEnumBitset<InstanceFlags>;

enum class AccelStructBuildFlags : uint8_t 
{
    None = 0,
    AllowUpdate,
    AllowCompaction,
    PreferFastTrace,
    PreferFastBuild,
    MinimizeMemory,
    PerformUpdate,
    AllowEmptyInstances,
    Count
};
using AccelStructBuildFlagsBitset = core::TEnumBitset<AccelStructBuildFlags>;

struct OpacityMicromapUsageCount 
{
    uint32_t count;
    uint32_t subdivisionLevel;
    OpacityMicromapFormat format;
};

}

struct BindingLayoutItem
{
    uint32_t slot;
    ResourceType type;
    uint8_t unused;
    uint16_t size;

    bool operator ==(const BindingLayoutItem& b) const
    {
        return slot == b.slot
            && type == b.type
            && size == b.size;
    }

    bool operator !=(const BindingLayoutItem& b) const
    {
        return !(*this == b);
    }

    static BindingLayoutItem Crate(uint32_t slot, ResourceType type)
    {
        BindingLayoutItem item = {};
        item.slot = slot;
        item.type = type;
        return item;
    }

    static BindingLayoutItem PushConstants(uint32_t slot, uint16_t size)
    {
        BindingLayoutItem item = {};
        item.slot = slot;
        item.type = ResourceType::ConstantBuffer;
        item.size = size;
        return item;
    }
};
using BindingLayoutItemArray = core::TStaticVector<BindingLayoutItem, MAX_BINDING_PER_LAYOUT>;

struct VulkanBindingOffsets
{
    uint32_t shaderResource = 0;
    uint32_t sampler = 128;
    uint32_t constantBuffer = 256;
    uint32_t unorderedAccess = 384;
};

struct SinglePassStereoState
{
    bool enabled = false;
    bool independentViewportMask = false;
    uint16_t renderargetIndexOffset = 0;

    bool operator ==(const SinglePassStereoState& b) const
    {
        return enabled == b.enabled
            && independentViewportMask == b.independentViewportMask
            && renderargetIndexOffset == b.renderargetIndexOffset;
    }

    bool operator !=(const SinglePassStereoState& b) const
    {
        return !(*this == b);
    }
};


struct RenderState
{
    BlendState blendState;
    DepthStencilState depthStencilState;
    RasterState rasterState;
    SinglePassStereoState singlePassStereo;
};

struct VariableRateShadingState
{
    bool enabled = false;
    VariableShadingRate shadingRate = VariableShadingRate::e1x1;
    ShadingRateCombiner pipelinePrimitiveCombiner = ShadingRateCombiner::Passthrough;
    ShadingRateCombiner imageCombiner = ShadingRateCombiner::Passthrough;

    bool operator ==(const VariableRateShadingState& b) const
    {
        return enabled == b.enabled
            && shadingRate == b.shadingRate
            && pipelinePrimitiveCombiner == b.pipelinePrimitiveCombiner
            && imageCombiner == b.imageCombiner;
    }

    bool operator !=(const VariableRateShadingState& b) const
    {
        return !(*this == b);
    }
};

struct VariableRateShadingFeatureInfo 
{
    uint32_t shadingRateImageTileSize;
};

enum class CommandListState : uint8_t 
{
    Initial,
    Open,
    Close
};

struct CommandListParameters
{
    bool enableImmediateExecution = true;
    size_t uploadChunkSize = 64 * 1024;
    size_t scratchChunkSize = 64 * 1024;
    size_t scratchMaxMemory = 1024 * 1024 * 1024;
    CommandQueue queueType = CommandQueue::Graphics;
};

struct PipelineCache
{
    core::FixedString<256> cacheName;
    void* cache;
    size_t cacheSize;
};


}
