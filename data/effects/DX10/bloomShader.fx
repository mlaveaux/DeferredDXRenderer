
#include "common.fx"

Texture2D colorTex;
Texture2D depthTex;

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
	
	color.rgb = colorTex.Sample(pointSampler, input.texcoord);
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
