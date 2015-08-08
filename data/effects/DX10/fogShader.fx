
#include "common.fx"

Texture2D colorTex;

struct PSInput
{
	float4 pos      : SV_POSITION;
	float2 texcoord : TEXCOORD1;
};

PSInput mainVS(appData input)
{
	PSInput output;
	
	output.pos      = float4(input.pos.xyz, 1.0f);
	output.texcoord = input.texcoord;

	return output;
}

float4 mainPS(PSInput input) : SV_Target0
{
	float4 color;
    float  depth;
	float fogFactor;

	color     = colorTex.Sample(pointSampler, input.texcoord);
	color.a   = (color.a) / 200;//(150.0f-color.a) / (150.0f - 50.0f);
	
    color.rgb = lerp(color.rgb, float3(0.6,0.6,0.6), color.a);
	color.a   = 1.0f;
	
	return color;
}

technique10 bloomShader
{
	pass p0
	{
		SetVertexShader(CompileShader(vs_4_0, mainVS()));
		SetPixelShader (CompileShader(ps_4_0, mainPS()));
	}
}
