//All the globally available data for effect pool

cbuffer perFrame
{
	float3 cameraPos : Camera;
}

cbuffer perObject
{
	float4x4 worldViewProj   : WorldViewProjection;
	float4x4 world           : World;
	float4x4 lightViewProj   : LightViewProjection;
}

struct appData
{
	float3 pos      : POSITION;
	float2 texcoord : TEXCOORD;
	float3 normal   : NORMAL;
};

SamplerState linearSampler
{
    Filter   = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

SamplerState pointSampler
{
    Filter   = MIN_MAG_MIP_POINT;
    AddressU = Wrap;
    AddressV = Wrap;
};