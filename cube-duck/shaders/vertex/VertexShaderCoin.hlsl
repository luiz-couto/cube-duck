
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

    // Make the coin spin around the Y axis
    float angle = time * 2.5;
    float cosAngle = cos(angle);
    float sinAngle = sin(angle);

    float4x4 rotationY = float4x4(
        cosAngle, 0, sinAngle, 0,
        0,        1,        0, 0,
       -sinAngle, 0, cosAngle, 0,
        0,        0,        0, 1
    );
    input.Pos = mul(input.Pos, rotationY);
    input.Normal = mul(input.Normal, (float3x3)rotationY);

    output.Pos = mul(input.Pos, input.World);
    output.Pos = mul(output.Pos, VP);

    output.Normal = mul(input.Normal, (float3x3)input.World);
    output.Tangent = mul(input.Tangent, (float3x3)input.World);
    output.TexCoord = input.TexCoord;

    output.LocalPos = input.Pos.xyz;

    return output;
}