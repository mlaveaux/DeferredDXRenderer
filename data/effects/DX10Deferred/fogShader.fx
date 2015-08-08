#include "include/common.fx"

Texture2D inputTex[2];

float fogStart = 10;
float fogEnd   = 200;
float3 fogColor = float3(0.7, 0.7, 0.7);
#include "include/depth.fx"

struct PSInput
{
	float4 pos      : SV_POSITION;
};

PSInput mainVS(appData input)
{
	PSInput output;
	
	output.pos      = float4(input.pos.xyz, 1.0f);

	return output;
}

float4 mainPS(PSInput input) : SV_Target0
{
	float4 color;
    float  depth;
	float fogFactor;
	
	color     = inputTex[0].Load(float3(input.pos.xy, 0), 0);
	depth     = makeLinear(inputTex[1].Load(float3(input.pos.xy, 0), 0).r);
	
	
	float depthVal = (fogEnd-depth) / (fogEnd-fogStart);
	depthVal = saturate(1 - depthVal);
		
    color.rgb = lerp(color.rgb, fogColor, depthVal);
	color.a   = 1.0f;
	
	return color;
}

technique10 fogShader
{
	pass p0
	{
		SetVertexShader(CompileShader(vs_4_0, mainVS()));
		SetPixelShader (CompileShader(ps_4_0, mainPS()));
	}
}
