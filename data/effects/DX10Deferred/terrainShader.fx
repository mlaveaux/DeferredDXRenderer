//Terrain shaders:
#include "include/common.fx"
#include "include/lighting.fx"
#include "include/shadow.fx"

int uscale = 1;
int vscale = 1;

Texture2D textureArray[4];
Texture2D blendTex;
Texture2D normalTex;

struct PSInput
{
	float4 pos      : SV_POSITION;
    float4 lpos     : TEXCOORD1;
	float2 texcoord : TEXCOORD2;
	float3 color    : COLOR;
};

PSInput mainVS(appData input)
{
	PSInput output;
	
	output.pos      = mul(float4(input.pos.xyz, 1.0f), worldViewProj);
	output.color    = calcDiffuse(mul(input.normal, (float3x3)world), normalize(dirLightPos));
	output.texcoord = input.texcoord;
	output.lpos     = mul(float4(input.pos.xyz, 1.0f), lightViewProj);

	return output;
}

float4 mainPS(PSInput input) : SV_Target0
{
	float4 color;
	
	float3 textureColor = 0.0f;
	float4 blendFactor = blendTex.Sample(linearSampler, input.texcoord);
	 
	if(blendFactor.r > 0.01f)
		textureColor  = textureArray[0].Sample(linearSampler, float2(input.texcoord.x * uscale, input.texcoord.y * vscale)).rgb * blendFactor.r;    
	if(blendFactor.g > 0.01f)
	    textureColor += textureArray[1].Sample(linearSampler, float2(input.texcoord.x * uscale, input.texcoord.y * vscale)).rgb * blendFactor.g;
	if(blendFactor.b > 0.01f)
	    textureColor += textureArray[2].Sample(linearSampler, float2(input.texcoord.x * uscale, input.texcoord.y * vscale)).rgb * blendFactor.b;
	if(blendFactor.a > 0.01f)
	    textureColor += textureArray[3].Sample(linearSampler, float2(input.texcoord.x * uscale, input.texcoord.y * vscale)).rgb * blendFactor.a;
	  
    color.rgb = input.color * calcShadowFactor(input.lpos) + ambientColor * ambientIntensity;
	color.rgb *= textureColor;
	color.a   = 1.0f;
	
	return color;
}

technique10 terrainShader
{
	pass p0
	{
		SetVertexShader(CompileShader(vs_4_0, mainVS()));
		SetPixelShader (CompileShader(ps_4_0, mainPS()));
	}
}
