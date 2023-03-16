
cbuffer ConstantBuffer : register(b0)
{
	matrix world;
	matrix view;
	matrix projection;
	float3 lightDirection;
}

struct VsInput
{
	float3 pos : POSITION;
	float3 norm : NORMAL;
};

struct VsOutput
{
	float4 pos : SV_Position;
	float4 norm : NORMAL;
};

VsOutput main(const VsInput input)
{
	VsOutput output = (VsOutput)0;
	output.pos = mul(float4(input.pos.xyz, 1), world);
	output.pos = mul(output.pos, view);
	output.pos = mul(output.pos, projection);
	output.norm = normalize(mul(float4(input.norm.xyz, 1), world));
	return output;
}
