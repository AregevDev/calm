#pragma pack_matrix(row_major)

struct PSInput
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float2 tex_coord : TEX_COORD;
    float3 tangent : TANGENT;
    float3 bitangent : BITANGENT;

    float3 frag_pos : FRAG_POS;
    float3 light_pos : LIGHT_POS;
};

cbuffer MaterialBuf : register(b0)
{
    float3 diffuse;
};

float4 ps_main(PSInput input) : SV_TARGET
{
    float3 light_color = float3(1.0, 1.0, 1.0);

    float ambient_factor = 0.1;
    float3 ambient = ambient_factor * light_color;

    float3 norm = normalize(input.normal);
    float3 light_dir = normalize(input.light_pos - input.frag_pos);
    float diff = max(dot(norm, light_dir), 0.0);
    float3 diffuse1 = diff * light_color;

    float specular_strength = 0.5;
    float3 view_dir = normalize(-input.frag_pos);
    float3 halfway = normalize(light_dir + view_dir);
    float spec = pow(max(dot(norm, halfway), 0.0), 32.0);
    float3 specular = specular_strength * spec * light_color;

    float3 result = (ambient + diffuse1 + specular) * diffuse;
    return float4(result, 1.0);
}
