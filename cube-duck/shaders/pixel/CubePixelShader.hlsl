cbuffer CubeMaterialCB : register(b0) {
    float3 TopColor;
    float3 BottomColor;
    float  HeightThreshold;

    float3 LightDirection;
    float  LightStrength;
};

struct PS_INPUT {
    float4 Pos       : SV_POSITION;
    float3 WorldPos  : TEXCOORD0;
    float3 Normal    : TEXCOORD1;
};

float4 PS(PS_INPUT input) : SV_Target0 
{
    float3 normal = normalize(input.Normal);

    // height for coloring
    float height = input.WorldPos.y;

    // top region detection
    bool topStrip = height > HeightThreshold;

    float3 baseColor = topStrip ? TopColor : BottomColor;

    // lighting
    float3 lightDir = normalize(LightDirection);
    float light = saturate(dot(normal, lightDir));

    float3 finalColor = baseColor * (0.3 + LightStrength * light);

    return float4(finalColor, 1.0);
};