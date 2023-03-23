
cbuffer ConstantBuffer : register(b0)
{
	matrix world;
	matrix view;
	matrix projection;
	float3 lightDirection;
	float3 cameraPosition;
}

SamplerState mySampler : register(s0);
Texture2D colorTexture : register(t0);
Texture2D normalTexture : register(t1);

struct PsInput
{
	float4 position : SV_Position;
	float4 worldPosition : POSITION;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 bitangent : BITANGENT;
	float2 uv : TEXCOORD;
};

static const float4 specularColor = { 0.25, 0.25, 0.2, 1 };
static const float specularPower = 20;

float4 main(const PsInput input) : SV_Target
{
	float3 normal = normalTexture.Sample(mySampler, input.uv).xyz * 2 - 1;

	float3 T = normalize(input.tangent);
	float3 B = normalize(input.bitangent);
	float3 N = normalize(input.normal);

	float3x3 TBN = transpose(float3x3(T, B, N));
	normal = mul(TBN, normal);
	normal = mul(world, normal);
	normal = normalize(normal);

	float diffuse = dot(lightDirection, normal) * 0.5 + 0.5;

	float3 viewDir = normalize(input.worldPosition.xyz - cameraPosition);
	float3 reflectLightDir = reflect(lightDirection, normal);

	float4 specular = pow(max(dot(reflectLightDir, viewDir), 0), specularPower) * specularColor;

	float4 color = colorTexture.Sample(mySampler, input.uv);

	color = specular + diffuse * color;

	return color;
}
