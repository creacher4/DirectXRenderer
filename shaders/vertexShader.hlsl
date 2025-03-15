cbuffer MatrixBuffer : register(b0)
{
    matrix world;
    matrix wvp;
};

struct VS_INPUT
{
    float3 position : POSITION;
    float3 normal   : NORMAL;
    float4 color    : COLOR;
    float2 texCoord : TEXCOORD0;
    float3 tangent  : TANGENT;
    float3 bitangent : BITANGENT;
};

struct PS_INPUT
{
    float4 position     : SV_POSITION;
    float4 color        : COLOR;
    float2 texCoord     : TEXCOORD0;
    float3 normal       : NORMAL;
    float3 tangent      : TANGENT;
    float3 bitangent    : BITANGENT;
    float3 worldPos     : TEXCOORD1;
};

PS_INPUT main(VS_INPUT input)
{
    PS_INPUT output;
    
    output.position = mul(float4(input.position, 1.0f), wvp);

    output.color = input.color;
    output.texCoord = input.texCoord;

    output.normal = normalize(mul(float4(input.normal, 0.0f), world).xyz);
    output.tangent = normalize(mul(float4(input.tangent, 0.0f), world).xyz);
    output.bitangent = normalize(mul(float4(input.bitangent, 0.0f), world).xyz);
    
    output.worldPos = mul(float4(input.position, 1.0f), world).xyz;
    
    return output;
}