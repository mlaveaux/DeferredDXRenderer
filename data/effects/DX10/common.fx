//All the globally available data for effect pool

float3 dirLightPos : Position
<	
	string Object = "dirLight0"; 
	string UIName = "Lamp 0 Position"; 
	string Space = "World";
> = { 0.5f, 0.5f, 0.5f };
	
float ambientIntensity
<	
	string UIName   = "Ambient Intensity";
	string UIWidget = "slider";	
	float UIMin = 0.0f;
   	float UIMax = 1.0f;
    	float UIStep = 0.05f;
> = 0.4f;

float3 ambientColor : Color = { 1.0f, 0.999f, 0.593f };

float3 cameraPos : Camera;

float4x4 worldViewProj   : WorldViewProjection;
float4x4 world           : World;
float4x4 lightViewProj   : LightViewProjection;

struct appData
{
	float3 pos      : POSITION;
	float2 texcoord : TEXCOORD;
	float3 normal   : NORMAL;
};

SamplerState linearSampler
{
    Filter   = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

SamplerState pointSampler
{
    Filter   = MIN_MAG_MIP_POINT;
    AddressU = Wrap;
    AddressV = Wrap;
};

//Common functions
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