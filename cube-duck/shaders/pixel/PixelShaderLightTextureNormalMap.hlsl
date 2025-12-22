Texture2D tex : register(t0);
Texture2D normalsTexture : register(t8);
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
    float3 tangent = normalize(input.Tangent);
    float3 binormal = normalize(cross(input.Normal, tangent));
    float3x3 TBN = float3x3(tangent, binormal, normal);

    // Sample and unpack normal map (stays in tangent space)
    float3 normalMapSample = normalsTexture.Sample(samplerLinear, input.TexCoord).rgb;
    float3 mapNormal = normalize(normalMapSample * 2.0 - 1.0);

    // Rotate lighting into tangent space using transpose(TBN)
    float3 lightDir = normalize(LightDirection);
    float3 localLightDir = mul(lightDir, transpose(TBN));

    float3 albedo = colour.rgb;
    float3 lightColor = LightColor * LightStrength;

    float NdotL = max(dot(mapNormal, localLightDir), 0.0);
    float3 diffuseBRDF = (albedo / 3.14159) * lightColor * NdotL;

    float3 ambient = albedo * 0.4;
    float3 finalColor = ambient + diffuseBRDF;

    return float4(finalColor, 1.0);
}