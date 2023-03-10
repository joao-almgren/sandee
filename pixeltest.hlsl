
struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float4 col : COLOR0;
};

float4 ps_main(VS_OUTPUT input) : SV_Target
{
	return input.col;
}
