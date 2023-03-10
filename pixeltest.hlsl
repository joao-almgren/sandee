
struct VsOutput
{
    float4 pos : SV_POSITION;
    float4 col : COLOR0;
};

float4 main(const VsOutput input) : SV_Target
{
	return input.col;
}
