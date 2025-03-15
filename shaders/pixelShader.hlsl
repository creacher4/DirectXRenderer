// documentation will be added for these shaders soon
// they really need it and honestly
// i might need to rewrite them a little
// but for now, they will do until i add shadow mapping or something and need to rewrite them

Texture2D diffuseTexture : register(t0);
Texture2D normalTexture : register(t1);
SamplerState textureSampler : register(s0);

#define LIGHT_DIRECTIONAL 0
#define LIGHT_POINT 1
#define LIGHT_SPOT 2

cbuffer LightBuffer : register(b1)
{
    float4 ambientColor;
    float4 diffuseColor;
    float3 lightDirection;
    float padding1;
    float3 lightPosition;
    float lightRange;
    float spotInnerCone;
    float spotOuterCone;
    int lightType;
    float lightIntensity;
}

cbuffer CameraBuffer : register(b2)
{
    float3 cameraPosition;
    float padding2;
}

struct PS_INPUT
{
    float4 position  : SV_POSITION;
    float4 color     : COLOR;
    float2 texCoord  : TEXCOORD0;
    float3 normal    : NORMAL;
    float3 tangent   : TANGENT;
    float3 bitangent : BITANGENT;
    float3 worldPos  : TEXCOORD1;
};

float3 CalcDirectionalLight(float3 normal, float3 viewDir)
{
    float NdotL = max(0.0f, dot(normal, -lightDirection));
    float3 diffuse = diffuseColor.rgb * NdotL;
    
    return diffuse;
}

float3 CalcPointLight(float3 normal, float3 worldPos, float3 viewDir)
{
    float3 lightVector = lightPosition - worldPos;
    
    float distance = length(lightVector);
    if(distance > lightRange)
        return float3(0.0f, 0.0f, 0.0f);

    lightVector = normalize(lightVector);
    
    float NdotL = max(0.0f, dot(normal, lightVector));
    float attenuation = 1.0f - saturate(distance / lightRange);
    attenuation = attenuation * attenuation;
    
    float3 diffuse = diffuseColor.rgb * NdotL * attenuation;
    return diffuse;
}

float3 CalcSpotLight(float3 normal, float3 worldPos, float3 viewDir)
{
    float3 lightVector = lightPosition - worldPos;

    float distance = length(lightVector);
    if(distance > lightRange)
        return float3(0.0f, 0.0f, 0.0f);

    lightVector = normalize(lightVector);

    float spotFactor = dot(-lightVector, normalize(lightDirection));

    if(spotFactor < spotOuterCone)
        return float3(0.0f, 0.0f, 0.0f);

    float NdotL = max(0.0f, dot(normal, lightVector));

    float attenuation = 1.0f - saturate(distance / lightRange);
    attenuation = attenuation * attenuation;

    if(spotFactor < spotInnerCone)
    {
        float spotRatio = (spotFactor - spotOuterCone) / (spotInnerCone - spotOuterCone);
        attenuation *= spotRatio;
    }

    float3 diffuse = diffuseColor.rgb * NdotL * attenuation;
    return diffuse;
}

float4 main(PS_INPUT input) : SV_Target
{
    float3 normalMapValue = normalTexture.Sample(textureSampler, input.texCoord).rgb;
    
    normalMapValue = normalMapValue * 2.0f - 1.0f;

    float3 N = normalize(input.normal);
    float3 T = normalize(input.tangent);
    float3 B = normalize(input.bitangent);

    T = normalize(T - dot(T, N) * N);
    B = cross(N, T);
    
    float3x3 TBN = float3x3(T, B, N);

    float3 normal = mul(normalMapValue, TBN);
    normal = normalize(normal);
    
    float3 viewDir = normalize(cameraPosition - input.worldPos);
    float3 lighting = ambientColor.rgb;

    if (lightType == LIGHT_DIRECTIONAL)
    {
        lighting += CalcDirectionalLight(normal, viewDir);
    }
    else if (lightType == LIGHT_POINT)
    {
        lighting += CalcPointLight(normal, input.worldPos, viewDir);
    }
    else if (lightType == LIGHT_SPOT)
    {
        lighting += CalcSpotLight(normal, input.worldPos, viewDir);
    }

    lighting *= lightIntensity;

    float4 texColor = diffuseTexture.Sample(textureSampler, input.texCoord);

    float4 finalColor = float4(lighting, 1.0f) * texColor;
    return finalColor;
}