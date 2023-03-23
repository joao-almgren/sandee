
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
	float4 col : COLOR;
	float2 uv : TEXCOORD;
};

struct VsOutput
{
	float4 pos : SV_Position;
	float4 norm : NORMAL;
	float4 col : COLOR;
	float2 uv : TEXCOORD;
};

VsOutput main(const VsInput input)
{
	VsOutput output = (VsOutput)0;
	output.pos = mul(float4(input.pos.xyz, 1), world);
	output.pos = mul(output.pos, view);
	output.pos = mul(output.pos, projection);
	output.norm = normalize(mul(float4(input.norm.xyz, 0), world));
	output.col = input.col;
	output.uv = input.uv;
	return output;
}
