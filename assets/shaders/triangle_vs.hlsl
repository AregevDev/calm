#pragma pack_matrix(row_major)

struct VSInput
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 tex_coord : TEX_COORD;
    float3 tangent : TANGENT;
    float3 bitangent : BITANGENT;
};

cbuffer CamBuf : register(b0)
{
    matrix mvp;
}

struct VSOutput
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float2 tex_coord : TEX_COORD;
    float3 tangent : TANGENT;
    float3 bitangent : BITANGENT;
};

VSOutput vs_main(VSInput input)
{
    VSOutput output;
    output.position = mul(float4(input.position, 1.0), mvp);
    output.normal = input.normal;
    output.tex_coord = input.tex_coord;
    output.tangent = input.tangent;
    output.bitangent = input.bitangent;

    return output;
}
