
cbuffer ConstantBuffer : register(b0)
{
	matrix world;
	matrix view;
	matrix projection;
	float3 lightDirection;
	float3 cameraPosition;
}

struct VsInput
{
	float3 position : POSITION;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 bitangent : BITANGENT;
	float3 uv : TEXCOORD;
};

struct VsOutput
{
	float4 position : SV_Position;
	float4 worldPosition : POSITION;
	float4 normal : NORMAL;
};

VsOutput main(const VsInput input)
{
	VsOutput output = (VsOutput)0;
	output.worldPosition = mul(float4(input.position.xyz, 1), world);
	output.position = mul(output.worldPosition, view);
	output.position = mul(output.position, projection);
	output.normal = normalize(mul(float4(input.normal.xyz, 0), world));
	return output;
}
