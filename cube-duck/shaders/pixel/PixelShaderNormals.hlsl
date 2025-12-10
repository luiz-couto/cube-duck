struct PS_INPUT {
    float4 Pos      : SV_POSITION;
    float3 WorldPos : TEXCOORD0;
    float3 Normal   : TEXCOORD1;
    float3 LocalPos  : TEXCOORD2;
};

float4 PS(PS_INPUT input) : SV_Target0 {
    return float4(abs(normalize(input.Normal)) * 0.9f, 1.0);
}
