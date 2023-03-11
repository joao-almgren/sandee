
cbuffer ConstantBuffer : register(b0)
{
    matrix world;
    matrix view;
    matrix proj;
    float4 vLightDir;
}

SamplerState mySampler : register(s0);
Texture2D myTexture : register(t0);

struct VsInput
{
    float4 pos : POSITION;
    float3 norm : NORMAL;
    float4 col : COLOR;
    float2 uv : TEXCOORD;
};

struct VsOutput
{
    float4 pos : SV_POSITION;
    float3 norm : NORMAL;
    float4 col : COLOR;
    float2 uv : TEXCOORD;
};

VsOutput main(const VsInput input)
{
    VsOutput output = (VsOutput)0;
    output.pos = mul(input.pos, world);
    output.pos = mul(output.pos, view);
    output.pos = mul(output.pos, proj);
    output.norm = mul(float4(input.norm, 1), world).xyz;
    output.col = input.col;
    output.uv = input.uv;
    return output;
}
