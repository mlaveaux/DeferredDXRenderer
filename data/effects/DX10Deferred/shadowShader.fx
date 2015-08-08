#include "common.fx"

Texture2D diffuseTex;

struct PSInput
{
	//Diffuse 
	float4 pos      : SV_POSITION;
	float2 texcoord : TEXCOORD1;
	float3 color    : COLOR;
	//SpecLight	
	float3 normal   : NORMAL;
	float3 viewDir  : VIEWDIR;
};

PSInput mainVS(appData input)
{
	PSInput output;
	
	output.pos      = mul(float4(input.pos.xyz,1.0f), worldViewProj);
	output.normal   = normalize(mul(input.normal, (float3x3)world));
	output.viewDir  = normalize(cameraPos - mul(input.pos, (float3x3)world) );
	
	output.color    = calcAmbientDiffuse(output.normal, normalize(dirLightPos));
	output.texcoord = input.texcoord;

	return output;
}

gBuffer mainPS(PSInput input)
{
	float4 color;
	
	color.rgb  = input.color;
	color.rgb *= diffuseTex.Sample(linearSampler, input.texcoord).rgb;
	color.rgb += float3(1.0f, 1.0f, 1.0f) * calcSpecular(input.viewDir, input.normal, normalize(dirLightPos), 50.0f); 
	color.a    = 1.0f;
	
	gBuffer output;
	output.color = color;
	output.depth = float4(input.pos.z * input.pos.w,0,0,1);

	return output;
}

technique10 shadowShader
{
	pass p0
	{
		SetVertexShader(CompileShader(vs_4_0, mainVS()));
		SetPixelShader (CompileShader(ps_4_0, mainPS()));
	}
}
