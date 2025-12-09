cbuffer staticMeshBuffer {
    float4x4 W;
    float4x4 VP;
};

struct VS_INPUT {
    float4 Pos : POSITION;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float2 TexCoord : TEXCOORD;
};

struct PS_INPUT {
    float4 Pos      : SV_POSITION;
    float3 WorldPos : TEXCOORD0;
    float3 Normal   : TEXCOORD1;
};

PS_INPUT VS(VS_INPUT input) {
    PS_INPUT output;

    float4 world = mul(input.Pos, W);
    output.Pos      = mul(world, VP);
    output.WorldPos = world.xyz;

    // normal transform
    output.Normal = normalize(mul(input.Normal, (float3x3)W));

    return output;
}
