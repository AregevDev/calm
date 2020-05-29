struct PSInput
{
    float4 position : SV_POSITION;
    float3 color : COLOR;
};

float4 ps_main(PSInput input) : SV_TARGET
{
    return float4(input.color.xyz, 1.0);
}
