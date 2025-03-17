// documentation has been created for this new lambertian based shader
// just need to update the documentation elsewhere and publish it

Texture2D diffuseTexture : register(t0);
Texture2D normalTexture : register(t1);
SamplerState textureSampler : register(s0);

// light types
// might need to fix this because my lights dont work like they should
// also i really wanna add debug boxes for the lights
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

// apply fresnel effect for a stylized rim lighting
float3 CalculateRimLight(float3 normal, float3 viewDir, float3 baseColor, float rimPower)
{
    float rim = 1.0f - saturate(dot(normal, viewDir));
    rim = pow(rim, rimPower);
    return rim * baseColor * 0.5f;
}

// stylized diffuse lighting using half-Lambert
float3 CalculateStylizedDiffuse(float3 normal, float3 lightDir, float3 lightColor)
{
    // half-Lambert diffuse (softer falloff)
    // talked about in the documentation
    float NdotL = dot(normal, lightDir) * 0.5f + 0.5f;
    NdotL = NdotL * NdotL; // contrasty
    
    return lightColor * NdotL;
}

// calculate specular with a stylized falloff
float3 CalculateStylizedSpecular(float3 normal, float3 lightDir, float3 viewDir, float3 specularColor, float power)
{
    float3 halfVector = normalize(lightDir + viewDir);
    float NdotH = max(0.0f, dot(normal, halfVector));
    float specular = pow(NdotH, power);
    
    // apply a smoother falloff
    specular = smoothstep(0.05f, 0.1f, specular) * specular;
    
    return specularColor * specular;
}

float3 CalculateDirectionalLight(float3 normal, float3 viewDir, float3 baseColor)
{
    float3 lightDir = normalize(-lightDirection);
    
    // stylized diffuse
    float3 diffuse = CalculateStylizedDiffuse(normal, lightDir, diffuseColor.rgb);
    // stylized specular
    float3 specular = CalculateStylizedSpecular(normal, lightDir, viewDir, float3(0.3f, 0.3f, 0.3f), 32.0f);
    
    return (diffuse + specular) * baseColor;
}

float3 CalculatePointLight(float3 normal, float3 worldPos, float3 viewDir, float3 baseColor)
{
    float3 lightVec = lightPosition - worldPos;
    float distance = length(lightVec);
    
    // early out if beyond range
    if (distance > lightRange)
        return float3(0.0f, 0.0f, 0.0f);
    
    float3 lightDir = normalize(lightVec);
    
    // stylized diffuse
    float3 diffuse = CalculateStylizedDiffuse(normal, lightDir, diffuseColor.rgb);
    // stylized specular
    float3 specular = CalculateStylizedSpecular(normal, lightDir, viewDir, float3(0.3f, 0.3f, 0.3f), 32.0f);
    
    // stylized attenuation with smoother falloff
    float attenFactor = saturate(1.0f - (distance / lightRange));
    float attenuation = attenFactor * attenFactor;
    
    return (diffuse + specular) * baseColor * attenuation;
}

float3 CalculateSpotLight(float3 normal, float3 worldPos, float3 viewDir, float3 baseColor)
{
    float3 lightVec = lightPosition - worldPos;
    float distance = length(lightVec);
    
    // early out if beyond range
    if (distance > lightRange)
        return float3(0.0f, 0.0f, 0.0f);
    
    float3 lightDir = normalize(lightVec);
    
    // spot cone calculation with stylized falloff
    float spotFactor = dot(-lightDir, normalize(lightDirection));
    if (spotFactor < spotOuterCone)
        return float3(0.0f, 0.0f, 0.0f);
    
    // stylized diffuse
    float3 diffuse = CalculateStylizedDiffuse(normal, lightDir, diffuseColor.rgb);
    // stylized specular
    float3 specular = CalculateStylizedSpecular(normal, lightDir, viewDir, float3(0.3f, 0.3f, 0.3f), 32.0f);
    
    // distance attenuation
    float attenFactor = saturate(1.0f - (distance / lightRange));
    float attenuation = attenFactor * attenFactor;
    
    // spot attenuation
    if (spotFactor < spotInnerCone)
    {
        float spotRatio = smoothstep(spotOuterCone, spotInnerCone, spotFactor);
        attenuation *= spotRatio;
    }
    
    return (diffuse + specular) * baseColor * attenuation;
}

float4 main(PS_INPUT input) : SV_Target
{
    // sample normal map and transform to world space
    float3 normalMapValue = normalTexture.Sample(textureSampler, input.texCoord).rgb;
    normalMapValue = normalMapValue * 2.0f - 1.0f;
    
    // create TBN matrix for tangent space to world space conversion
    float3 N = normalize(input.normal);
    float3 T = normalize(input.tangent);
    float3 B = normalize(input.bitangent);
    
    // ensure tangent is perpendicular to normal
    T = normalize(T - dot(T, N) * N);
    B = cross(N, T);
    
    float3x3 TBN = float3x3(T, B, N);
    
    // apply normal map
    float3 normal = normalize(mul(normalMapValue, TBN));
    
    // get view direction
    float3 viewDir = normalize(cameraPosition - input.worldPos);
    
    // sample base color from diffuse texture
    float4 texColor = diffuseTexture.Sample(textureSampler, input.texCoord);
    float3 baseColor = texColor.rgb;
    
    // start with ambient lighting
    float3 finalColor = ambientColor.rgb * baseColor;
    
    // add appropriate light contribution based on light type
    if (lightType == LIGHT_DIRECTIONAL)
    {
        finalColor += CalculateDirectionalLight(normal, viewDir, baseColor);
    }
    else if (lightType == LIGHT_POINT)
    {
        finalColor += CalculatePointLight(normal, input.worldPos, viewDir, baseColor);
    }
    else if (lightType == LIGHT_SPOT)
    {
        finalColor += CalculateSpotLight(normal, input.worldPos, viewDir, baseColor);
    }
    
    // apply light intensity
    finalColor *= lightIntensity;
    
    // add stylized rim lighting
    float3 rimLight = CalculateRimLight(normal, viewDir, float3(0.3f, 0.4f, 0.5f), 3.0f);
    finalColor += rimLight;
    
    // apply a very subtle toon shading by discretizing the final color
    // comment out this line for a smoother look
    // i have it out commented because i dont like the look of it
    // reminds me of lethal company tbh
    // finalColor = floor(finalColor * 5.0f) / 5.0f;
    
    return float4(finalColor, texColor.a);
}