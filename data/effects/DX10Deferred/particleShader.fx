#include "common.fx"

Texture2D diffuseTex;

struct particleData
{
	float3 pos      : POSITION;
	float2 size     : SIZE;
};

struct PSInput
{
	float4 pos      : SV_POSITION;
	float2 texcoord : TEXCOORD1;
	float3 color    : COLOR;
};

PSInput mainVS(appData input)
{	
	return input;
}

[maxvertexcount(4)]
void mainGS( point GSInput base[1], inout TriangleStream<PSInput> stream )
{
	PSInput v;
    v.pos = mul(float4(base[0].pos, 1.0f), worldViewProj);
	v.texcoord = float2(0.0f, 0.0f);
	
	stream.Append(v);
	
	v.pos.x += base[0].size.x;
	v.texcoord = float2(1.0f, 0.0f); 
	
	stream.Append(v);
	
	v.pos.y += base[0].size.y;
	v.texcoord = float2(1.0f, 1.0f);
	
	stream.Append(v);
	
	v.pos.x -= base[0].size.x;
	v.texcoord = float2(0.0f, 1.0f);
	
	stream.Append(v);	
}

float4 mainPS(PSInput input) : SV_Target0
{
	float4 color;
	
	color.rgb = diffuseTex.Sample(linearSampler, input.texcoord).rgb;
	color.a   = 1.0f;

	return color;
}

technique10 diffuseShader
{
	pass p0
	{
		SetVertexShader(   CompileShader(vs_4_0, mainVS()));
        SetGeometryShader( CompileShader( gs_4_0, mainGS()));
		SetPixelShader (   CompileShader(ps_4_0, mainPS()));
	}
}
