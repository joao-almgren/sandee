
cbuffer ConstantBuffer : register(b0)
{
	matrix world;
	matrix view;
	matrix projection;
}

struct VsInput
{
	float3 pos : POSITION;
	float2 uv : TEXCOORD;
};

struct VsOutput
{
	float4 pos : SV_Position;
	float2 uv : TEXCOORD;
};

VsOutput main(const VsInput input)
{
	VsOutput output = (VsOutput)0;
	output.pos = mul(float4(input.pos.xyz, 1), world);
	output.pos = mul(output.pos, view);
	output.pos = mul(output.pos, projection);
	output.uv = input.uv;
	return output;
}
