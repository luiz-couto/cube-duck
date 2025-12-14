Texture2D tex : register(t0);
SamplerState samplerLinear : register(s0);

cbuffer BRDFLightCB : register(b0) {
    float3 LightDirection;
    float3 LightColor;
    float  LightStrength;
}

struct PS_INPUT {
    float4 Pos : SV_POSITION;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float2 TexCoord : TEXCOORD;
    float3 LocalPos  : LOCAL_POS;
};

float4 PS(PS_INPUT input) : SV_Target0 {
    float4 colour = tex.Sample(samplerLinear, input.TexCoord);
    float3 normal = normalize(input.Normal);

    float3 albedo = colour.rgb;
    float3 lightDir = normalize(LightDirection);
    float3 lightColor = LightColor * LightStrength;

    float NdotL = max(dot(normal, lightDir), 0.0);  // max(ω_i · N, 0)
    float3 diffuseBRDF = (albedo / 3.14159) * lightColor * NdotL;

    float3 ambient = albedo * 0.2;  // Ambient light to prevent pure black
    float3 finalColor = ambient + diffuseBRDF;

    return float4(finalColor, 1.0);
}