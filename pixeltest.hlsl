
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
    float4 pos : SV_Position;
    float3 norm : NORMAL;
    float4 col : COLOR;
    float2 uv : TEXCOORD;
};

float4 main(const VsOutput input) : SV_Target
{
    float light = saturate(dot(input.norm, vLightDir.xyz));
    float diffuse = myTexture.Sample(mySampler, input.uv);
    float3 color = (input.col.xyz + diffuse) * light;
    return float4(color.xyz, 1);
}
