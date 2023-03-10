
struct VsOutput
{
    float4 pos : SV_POSITION;
    float4 col : COLOR0;
};

VsOutput main(const float4 pos : POSITION, const float4 col : COLOR)
{
    VsOutput output = (VsOutput)0;
    output.pos = pos;
    output.col = col;
	return output;
}
