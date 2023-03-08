
struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float4 col : COLOR0;
};

VS_OUTPUT vs_main(float4 pos : POSITION, float4 col : COLOR)
{
    VS_OUTPUT Out;
    Out.pos = pos;
    Out.col = col;
	return Out;
}
