Texture2D myTexture : register(t0);
SamplerState mySampler : register(s0);

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float4 color    : COLOR;
    float2 texCoord : TEXCOORD0;
    float3 normal   : NORMAL;
};

cbuffer LightBuffer : register(b1)
{
    float4 ambientColor;
    float4 diffuseColor;
    float3 lightDirection;
    float padding;
};

float4 main(PS_INPUT input) : SV_Target
{
    // Sample the texture.
    float4 texColor = myTexture.Sample(mySampler, input.texCoord);
    
    // Calculate diffuse lighting using a simple Lambertian model.
    float lightIntensity = saturate(dot(input.normal, -lightDirection));
    float4 diffuse = diffuseColor * lightIntensity;
    
    // Combine ambient and diffuse lighting.
    float4 finalColor = (ambientColor + diffuse) * input.color;
    
    return texColor * finalColor;
}
