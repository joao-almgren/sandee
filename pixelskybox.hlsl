
cbuffer ConstantBuffer : register(b0)
{
	matrix world;
	matrix view;
	matrix projection;
}

SamplerState mySampler : register(s0);
Texture2D myTexture : register(t0);

struct PsInput
{
	float4 pos : SV_Position;
	float2 uv : TEXCOORD;
};

float4 main(const PsInput input) : SV_Target
{
	float4 diffuse = myTexture.Sample(mySampler, input.uv);
	return diffuse;
}
