struct VSInput
{
    float3 position : POSITION;
    float3 color : COLOR;
};

cbuffer CamBuf : register(b0)
{
    matrix mvp;
}

struct VSOutput
{
    float4 position : SV_POSITION;
    float3 color : COLOR;
};

VSOutput vs_main(VSInput input)
{
    VSOutput output;
    output.position = mul(mvp, float4(input.position, 1.0));
    output.color = input.color;

    return output;
}
