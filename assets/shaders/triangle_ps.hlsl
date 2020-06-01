#pragma pack_matrix(row_major)

struct PSInput
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float2 tex_coord : TEX_COORD;
    float3 tangent : TANGENT;
    float3 bitangent : BITANGENT;
};

cbuffer MaterialBuf : register(b0)
{
    float3 diffuse;
};

float4 ps_main(PSInput input) : SV_TARGET
{
    return float4(diffuse, 1.0);
}

