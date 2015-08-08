
float3 dirLightPos = { 0.5f, 0.5f, 0.5f };

float  ambientIntensity = 0.2f;
float3 ambientColor : Color = { 1.0f, 0.999f, 0.593f };

float3 calcDiffuse(float3 normal, float3 dirLight)
{
    return (1.0f - ambientIntensity) * saturate(ambientColor * dot(normal, dirLight));
}

float3 calcAmbientDiffuse(float3 normal, float3 dirLight)
{
	return calcDiffuse(normal, dirLight) + ambientColor * ambientIntensity;
}

float calcSpecular(float3 viewDir, float3 normal, float3 dirLight, float shininess)
{
	float3 halfVector = normalize(viewDir + dirLight);
	float  reflect    = dot(normal, halfVector);
	
	if(reflect > 0.5f)
		return pow(reflect, shininess);
	else
		return 0;
}