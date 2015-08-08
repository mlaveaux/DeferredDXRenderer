#include "include/common.fx"
#include "include/lighting.fx"
#include "include/depth.fx"

Texture2D normalTex;
Texture2D depthTex;
Texture2D reflectTex;

matrix reflectViewProj : ReflectViewProj;

float2 offset;

struct PSInput
{
	float4 pos       : SV_POSITION;
	float4 rPos      : POSITION1;
	float3 viewDir   : POSITION2;
	float2 texcoord  : TEXCOORD0;
	float2 texcoord1 : TEXCOORD1;
};

PSInput mainVS(appData input)
{
	PSInput output;
	
	output.pos      = mul(float4(input.pos.xyz, 1.0f), worldViewProj);
	output.viewDir  = normalize(cameraPos - mul(input.pos, (float3x3)world));
	output.rPos     = mul(float4(input.pos.xyz, 1.0f), reflectViewProj);
	
	output.texcoord = 5.0f * input.texcoord;	
	output.texcoord += offset;
	
	output.texcoord1 = output.texcoord * 4.0f;
	
	return output;
}

float4 mainPS(PSInput input) : SV_Target0
{
	float4 color;
	//Load the normal variable and make the bounds [-1,1].
	float3 normal = normalize(((2.0f * normalTex.Sample(linearSampler, input.texcoord).rgb - 1.0f) +
							   (2.0f * normalTex.Sample(linearSampler, input.texcoord1).rgb - 1.0f)) / 2);
	float  depth  = makeLinear(depthTex.Load(float3(input.pos.xy, 0), 0).r) - input.pos.z * input.pos.w;
	
	//re-homogenize position after interpolation
	float2 reflectCoords = input.rPos.xy /= input.rPos.w;
	
	reflectCoords.x =  reflectCoords.x*0.5 + 0.5;
	reflectCoords.y = -reflectCoords.y*0.5 + 0.5;
	
	reflectCoords.xy += normal.xy * 0.05f;
	
	float3 reflectColor = reflectTex.Sample(linearSampler, reflectCoords.xy);
			
	float depthVal = (20-depth) / (20);
	depthVal = saturate(1 - depthVal);
	
	color.rgb = reflectColor + float3(1.0f, 1.0f, 1.0f) * calcSpecular(input.viewDir, normal, normalize(dirLightPos), 25.0f);
			
	color.a   = depthVal;
		
	return color;
}

technique10 oceanShader
{
	pass p0
	{
		SetVertexShader(CompileShader(vs_4_0, mainVS()));
		SetPixelShader (CompileShader(ps_4_0, mainPS()));
	}
}
