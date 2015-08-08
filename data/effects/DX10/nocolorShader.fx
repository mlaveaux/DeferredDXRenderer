/*
% Shadowed material shader.
% version 0.1
*/

#include "shadow.fx"

struct PSInput
{
	float4 pos      : SV_POSITION;
};

PSInput mainVS(appData input)
{
	PSInput output;
	
	output.pos = mul(float4(input.pos,1.0f), lightViewProj);

	return output;
}

technique10 nocolorShader
{	
	pass p0
	{		
		SetVertexShader(CompileShader(vs_4_0, mainVS()));
		SetPixelShader (NULL);
	}	
}
