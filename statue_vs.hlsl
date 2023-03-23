
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
	float4 position : POSITION;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 bitangent : BITANGENT;
	float2 uv : TEXCOORD;
};

struct VsOutput
{
	float4 position : SV_Position;
	float4 worldPosition : POSITION;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 bitangent : BITANGENT;
	float2 uv : TEXCOORD;
};

VsOutput main(const VsInput input)
{
	VsOutput output = (VsOutput)0;
	output.worldPosition = mul(input.position, world);
	output.position = mul(output.worldPosition, view);
	output.position = mul(output.position, projection);

	output.normal = input.normal;
	output.tangent = input.tangent;
	output.bitangent = input.bitangent;

	output.uv = input.uv;
	return output;
}
