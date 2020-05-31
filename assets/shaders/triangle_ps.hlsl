#pragma pack_matrix(row_major)

struct PSInput
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float2 tex_coord : TEX_COORD;
    float3 tangent : TANGENT;
    float3 bitangent : BITANGENT;
};

float4 ps_main(PSInput input) : SV_TARGET
{
    return float4(abs(input.normal), 1.0);
}
