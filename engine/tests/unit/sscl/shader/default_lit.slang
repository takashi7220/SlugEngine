#include "include1st.slang"
#include "gbuffer.slang"

cbuffer MaterialCB : register(b3)
{
    float4 gBaseColor;
    float3 gEmissiveColor;
    float gMetallicFactor;
    float gRoughnessFactor;
    float gSpecularFactor;
    float gOcclusionFactor;
}

Texture2D<float4> baseColorTexture : register(t0);
Texture2D<float4> normalTexture : register(t1);
Texture2D<float4> parameterTexture : register(t2);
Texture2D<float4> emissiveTexture : register(t3);

SamplerState linearSampler      : register(s0);

[shader("vertex")]
VSOutput VSMain(VSInput input)
{
    VSOutput output = CalculateVertex(input);
    return output;
}

[shader("fragment")]
PSOutput PSMain(VSOutput input)
{

}