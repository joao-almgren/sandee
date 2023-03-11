
struct VsOutput
{
    float4 pos : SV_Position;
    float4 col : COLOR;
};

float4 main(const VsOutput input) : SV_Target
{
	return input.col;
}
