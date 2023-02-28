float4 main(float2 pos : Position0) : SV_Position
{
	return float4(pos.x, pos.y, 0, 1);
}
