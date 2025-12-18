
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
    
    float cosT = cos(time);
    float sinT = sin(time);
    float4x4 rotationX = float4x4(
        1,     0,     0, 0,
        0,  cosT,  sinT, 0,
        0, -sinT,  cosT, 0,
        0,     0,     0, 1
    );

    float4 centeredPos = float4(input.Pos.xyz - float3(0, 2.0, 0), 1.0);
    float4 rotatedPos = mul(centeredPos, rotationX);
    rotatedPos = float4(rotatedPos.xyz + float3(0, 2.0, 0), 1.0);
    
    output.LocalPos = rotatedPos.xyz;
    output.TexCoord = input.TexCoord;

    output.Pos = mul(rotatedPos, input.World);
    output.Pos = mul(output.Pos, VP);

    output.Normal = mul(input.Normal, (float3x3)rotationX);
    output.Normal = mul(output.Normal, (float3x3)input.World);
    output.Tangent = mul(input.Tangent, (float3x3)rotationX);
    output.Tangent = mul(output.Tangent, (float3x3)input.World);

    return output;
}