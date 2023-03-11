
cbuffer ConstantBuffer : register(b0)
{
    matrix world;
    matrix view;
    matrix proj;
}

struct VsInput
{
    float4 pos : POSITION;
    float4 col : COLOR;
};

struct VsOutput
{
    float4 pos : SV_POSITION;
    float4 col : COLOR;
};

VsOutput main(const VsInput input)
{
    VsOutput output = (VsOutput)0;
    output.pos = mul(input.pos, world);
    output.pos = mul(output.pos, view);
    output.pos = mul(output.pos, proj);
    output.col = input.col;
	return output;
}
