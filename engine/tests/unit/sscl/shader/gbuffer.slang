#ifndef SLUG_GBUFFER
#define SLUG_GBUFFER

cbuffer ObjectCB : register(b2)
{
    float4x4 gWorld;
}

struct VSInput
{
    float3 position  : POSITION;
    float3 normal    : NORMAL;
    float2 uv0       : TEXCOORD0;
    float2 uv1       : TEXCOORD1;
    float2 uv2       : TEXCOORD2;
    float2 uv3       : TEXCOORD3;
    float4 tangent   : TANGENT;
    float4 binormal  : BINORMAL;
    uint4  boneIndices : BLENDINDICES;
    float4 boneWeights : BLENDWEIGHT;
};

struct VSOutput
{
    float4 svPosition : SV_Position;

    float3 position  : TEXCOORD0;
    float3 normal    : TEXCOORD1;
    float3 tangent   : TEXCOORD2;
    float3 binormal  : TEXCOORD3;
    float2 texcoord0 : TEXCOORD4;
    float2 texcoord1 : TEXCOORD5;
    float2 texcoord2 : TEXCOORD6;
    float2 texcoord3 : TEXCOORD7;
};

struct GBufferOutput
{
    float3 baseColor;
    float opacity;
    float3 worldNormal;
    float roughnessFactor;
    float metallicFactor;
    float specularFactor;
    float3 motionVector;
}

struct PSOutput
{
    float4 gbuffer0;
    float4 gbuffer1;
    float4 gbuffer2;
    float4 gbuffer3;
    float4 gbuffer4;
}

VSOutput CalculateDefaultVSOutput(VSInput input)
{
    float4 positionWS = mul(float4(input.position, 1.0f), gWorld);

    output.position = positionWS.xyz;
    output.svPosition = mul(positionWS, gViewProj);

    output.normalWS = SafeNormalize(mul(float4(input.normal, 0.0f), gWorld).xyz);
    float3 tangentWS = SafeNormalize(mul(float4(input.tangent.xyz, 0.0f), gWorld).xyz);
    float3 binormalWS = SafeNormalize(mul(float4(input.binormal, 0.0f), gWorld).xyz);

    output.tangentWS  = tangentWS
    output.binormalWS = binormalWS;

    output.uv0 = input.uv0;
    output.uv1 = input.uv1;
    output.uv2 = input.uv2;
    output.uv3 = input.uv3;
    return output;
}

PSOutput EncodePSOutput(GBuffer gbuffer)
{

}

#endif