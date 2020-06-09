#pragma pack_matrix(row_major)

struct VSInput
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 tex_coord : TEX_COORD;
    float3 tangent : TANGENT;
    float3 bitangent : BITANGENT;
};

cbuffer CameraMatrices : register(b0)
{
    matrix v;
    matrix mv;
    matrix mvp;
    matrix n;
}

struct VSOutput
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float2 tex_coord : TEX_COORD;
    float3 tangent : TANGENT;
    float3 bitangent : BITANGENT;

    float3 frag_pos : FRAG_POS;
    float3 light_dir : LIGHT_DIR;
};

VSOutput vs_main(VSInput input)
{
    float3 light_pos = float3(9.0, 9.0, -9.0);

    VSOutput output;
    output.position = mul(float4(input.position, 1.0), mvp);
    output.normal = mul(input.normal, (float3x3) n);
    output.tex_coord = input.tex_coord;
    output.tangent = input.tangent;
    output.bitangent = input.bitangent;

    output.frag_pos = (float3) mul(float4(input.position, 1.0), mv);
    output.light_dir = (float3) mul(float4(light_pos, 1.0), v);

    return output;
}
