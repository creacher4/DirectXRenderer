Texture2D myTexture : register(t0);
SamplerState mySampler : register(s0);

struct PS_INPUT {
    float4 position : SV_POSITION;
    float4 color    : COLOR;
    float2 texCoord : TEXCOORD0;
};

float4 main(PS_INPUT input) : SV_Target {
    float4 texColor = myTexture.Sample(mySampler, input.texCoord);
    return texColor * input.color; // Multiply texture by vertex color for modulation
}
