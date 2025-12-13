
cbuffer staticMeshBuffer {
    float4x4 W;
    float4x4 VP;
    float4x4 PLAYER_POS;
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
};

PS_INPUT VS(VS_INPUT input) {
    PS_INPUT output;

    float4 worldPos = mul(input.Pos, input.World);
    float3 playerPos = float3(PLAYER_POS[3][0], PLAYER_POS[3][1], PLAYER_POS[3][2]);

    float3 grassToPlayer = worldPos.xyz - playerPos.xyz;
    float dist = length(grassToPlayer);

    float bendRadius = 2;
    float bendStrength = 0.7;

    if (dist < bendRadius && dist > 0.001) {
        float bendFactor = 1.0 - (dist / bendRadius);
        bendFactor = smoothstep(0.0, 1.0, bendFactor);
        
        float heightFactor = input.Pos.y;
        float3 bendDir = normalize(grassToPlayer);
        
        float displacement = bendFactor * bendStrength * heightFactor;
        worldPos.xyz += bendDir * displacement;
    }

    output.Pos = mul(worldPos, VP);
    output.Normal = mul(input.Normal, (float3x3)input.World);
    output.Tangent = mul(input.Tangent, (float3x3)input.World);
    output.TexCoord = input.TexCoord;
    return output;
}