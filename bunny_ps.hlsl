
cbuffer ConstantBuffer : register(b0)
{
	matrix world;
	matrix view;
	matrix projection;
	float3 lightDirection;
	float3 cameraPosition;
}

SamplerState mySampler : register(s0);
Texture2D myTexture : register(t0);

struct PsInput
{
	float4 position : SV_Position;
	float4 worldPosition : POSITION;
	float4 normal : NORMAL;
	float2 uv : TEXCOORD;
};

static const float4 specularColor = { 0.25, 0.25, 0.2, 1 };
static const float specularPower = 20;

float4 main(const PsInput input) : SV_Target
{
	float3 viewDir = normalize(input.worldPosition.xyz - cameraPosition);
	float3 reflectLightDir = reflect(lightDirection, input.normal.xyz);

	float4 specular = pow(max(dot(reflectLightDir, viewDir), 0), specularPower) * specularColor;

	float diffuse = saturate(dot(lightDirection, normalize(input.normal.xyz)));

	float4 color = myTexture.Sample(mySampler, input.uv);

	color = diffuse * color + specular;

	return color;
}
