cbuffer ConstantBuffer : register(b0)
{
    matrix wvp; // Must match struct ConstantBuffer { XMMATRIX wvp; } in C++
};

struct VS_INPUT
{
    float3 position : POSITION;
    float4 color    : COLOR;
};

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float4 color    : COLOR;
};

PS_INPUT main(VS_INPUT input)
{
    PS_INPUT output;
    // Multiply by wvp to go from object space -> world space -> view space -> projection space
    output.position = mul(float4(input.position, 1.0f), wvp);
    output.color = input.color;
    return output;
}
