cbuffer ConstantBuffer : register(b0) {
    matrix wvp;
};

struct VS_INPUT {
    float3 position : POSITION;
    float4 color    : COLOR;
    float2 texCoord : TEXCOORD0;
};

struct PS_INPUT {
    float4 position : SV_POSITION;
    float4 color    : COLOR;
    float2 texCoord : TEXCOORD0;
};

PS_INPUT main(VS_INPUT input) {
    PS_INPUT output;
    output.position = mul(float4(input.position, 1.0f), wvp);
    output.color = input.color;
    output.texCoord = input.texCoord;
    return output;
}
