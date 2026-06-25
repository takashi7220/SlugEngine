#ifndef SLUG_INCLUDE1ST
#define SLUG_INCLUDE1ST

cbuffer GlobalCB : register(b0)
{

}

cbuffer ViewCB : register(b1)
{
    float4x4 gView;
    float4x4 gInvView;
    float4x4 gProj;
    float4x4 gInvProj;
    float4x4 gViewProj;
    float4x4 gInvViewProj;
    float2 gResolution;
    float3 gCameraPosition;
}

#endif