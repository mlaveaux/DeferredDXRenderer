#include "common.fx"

Texture2D shadowMap;

//Compare Sampler
SamplerComparisonState cmpSampler
{
   // sampler state
   Filter = COMPARISON_MIN_MAG_MIP_LINEAR;
   AddressU = MIRROR;
   AddressV = MIRROR;

   // sampler comparison state
   ComparisonFunc = LESS_EQUAL;
};

//Calculate shadowMap
float calcShadowFactor(float4 lpos)
{
    //re-homogenize position after interpolation
	lpos.xyz /= lpos.w;

	//if position is not visible to the light - dont illuminate it
	//results in hard light frustum
	if( lpos.x < -1.0f || lpos.x > 1.0f ||
	    lpos.y < -1.0f || lpos.y > 1.0f ||
	    lpos.z < 0.0f  || lpos.z > 1.0f )
        return 1;

	//transform clip space coord to texture space coords (-1:1 to 0:1)
	lpos.x = lpos.x/2 + 0.5;
	lpos.y = lpos.y/-2 + 0.5;

    lpos.z -= 0.01f;

	//sample shadow map - point sampler
	float shadowFactor = shadowMap.SampleCmpLevelZero( cmpSampler, lpos.xy, lpos.z );

    return shadowFactor;
}
