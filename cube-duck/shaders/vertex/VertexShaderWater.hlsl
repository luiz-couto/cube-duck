
cbuffer staticMeshBuffer {
    float4x4 W;
    float4x4 VP;
    float time;
};

struct VS_INPUT {
    float4 Pos : POSITION;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float2 TexCoord : TEXCOORD;
    float4x4 World : WORLD;
};

struct PS_INPUT {
    float4 Pos      : SV_POSITION;
    float3 Normal   : NORMAL;
    float3 Tangent  : TANGENT;
    float2 TexCoord : TEXCOORD;
    float3 LocalPos  : LOCAL_POS;
};

PS_INPUT VS(VS_INPUT input) {
    PS_INPUT output;
    float a1 = 0.1;
    float a2 = 0.15;

    float f1 = 0.5;
    float f2 = 2.0;

    float s1 = 1.0;
    float s2 = 1.5;

    float newY = input.Pos.y + a1 * sin(f1 * input.Pos.x + s1 * time) + a2 * cos(f2 * input.Pos.z + s2 * time);
    input.Pos.y = newY;
    
    output.Pos = mul(input.Pos, input.World);
    output.Pos = mul(output.Pos, VP);

    output.Normal = mul(input.Normal, (float3x3)input.World);
    output.Tangent = mul(input.Tangent, (float3x3)input.World);
    output.TexCoord = input.TexCoord;

    output.LocalPos = input.Pos.xyz;

    return output;
}