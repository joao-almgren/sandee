
SamplerState mySampler : register(s0);
Texture2D myTexture : register(t0);

struct VsOutput
{
    float4 pos : SV_Position;
    float4 col : COLOR;
    float2 uv : TEXCOORD;
};

float4 main(const VsOutput input) : SV_Target
{
    return myTexture.Sample(mySampler, input.uv) * input.col;
}
