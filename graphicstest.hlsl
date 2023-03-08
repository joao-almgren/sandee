
struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float4 col : COLOR;
};

VS_OUTPUT vs_main(float3 pos : POSITION, float3 col : COLOR)
{
    VS_OUTPUT Out;
    Out.pos = float4(pos.xyz, 1);
    Out.col = float4(col.rgb, 1);
    return Out;
}

float4 ps_main(float4 col : COLOR) : SV_TARGET
{
    return col;
}

technique Standard
{
    pass Pass0
    {
        VertexShader = compile vs_4_0 vs_main();
        PixelShader = compile ps_4_0 ps_main();
    }
}
