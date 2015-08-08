#include "include/common.fx"

Texture2D diffuseTex;

struct PSInput
{
	//Diffuse 
	float4 pos      : SV_POSITION;
	float2 texcoord : TEXCOORD1;
};

PSInput mainVS(appData input)
{
	PSInput output;
	
	//input.pos.x += cameraPos.x;
	//input.pos.z += cameraPos.y;
	
	output.pos      = mul(float4(input.pos.xyz, 1.0f), worldViewProj);
	output.texcoord = input.texcoord;

	return output;
}

float4 mainPS(PSInput input) : SV_Target0
{
	float4 color;
	
	color.rgb = diffuseTex.Sample(linearSampler, input.texcoord).rgb;
	color.a    = 1.0f;
	
	return color;
}

technique10 diffuseShader
{
	pass p0
	{
		SetVertexShader(CompileShader(vs_4_0, mainVS()));
		SetPixelShader (CompileShader(ps_4_0, mainPS()));
        SetGeometryShader(NULL);
	}
}
