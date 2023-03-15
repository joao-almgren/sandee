
cbuffer ConstantBuffer : register(b0)
{
	matrix world;
	matrix view;
	matrix projection;
	float3 lightDirection;
}

struct PsInput
{
	float4 pos : SV_Position;
	float3 norm : NORMAL;
};

float4 main(const PsInput input) : SV_Target
{
	float light = saturate(dot(input.norm, lightDirection));
	//return float4(light, light, light, 1);
	return (float4)1;
}
