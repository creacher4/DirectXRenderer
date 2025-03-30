// documentation has been created for this new lambertian based shader
// just need to update the documentation elsewhere and publish it

Texture2D diffuseTexture : register(t0);
Texture2D normalTexture : register(t1);
SamplerState textureSampler : register(s0);

// light types
// might need to fix this because my lights dont work like they should
// also i really wanna add debug boxes for the lights

// support for multiple lights
#define LIGHT_DIRECTIONAL 0
#define LIGHT_POINT 1
#define LIGHT_SPOT 2

// maximum number of lights
#define MAX_LIGHTS 8

// so instead of a buffer, convert to a struct for light data
struct LightData
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
};

// modified light buffer to support multiple lights
cbuffer LightBuffer : register(b1)
{
    LightData lights[MAX_LIGHTS];
    int activeLightCount;
    float3 padding;
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
float3 CalculateRimLight(float3 normal, float3 viewDir, float3 rimColor, float rimPower, float rimStrength)
{
    float rim = 1.0f - saturate(dot(normal, viewDir));
    rim = pow(rim, rimPower) * rimStrength;

    // add small variation based on view angle
    float viewAngle = abs(dot(float3(0, 1, 0), viewDir));
    rim *= lerp(0.7f, 0.5f, viewAngle);

    // instead of a base color, return the rim color
    return rim * rimColor;
}

// stylized diffuse lighting using half-Lambert
float3 CalculateStylizedDiffuse(float3 normal, float3 lightDir, float3 lightColor)
{
    // half-Lambert diffuse (softer falloff)
    // talked about in the documentation
    float NdotL = dot(normal, lightDir);
    float halfLambert = NdotL * 0.5f + 0.5f;
    halfLambert = pow(halfLambert, 2.0f); // using pow method for contrast
    
    return lightColor * halfLambert;
}

// improved specular with blinn-phong
float3 CalculateStylizedSpecular(float3 normal, float3 lightDir, float3 viewDir, float3 specularColor, float power)
{
    float3 halfVector = normalize(lightDir + viewDir);
    float NdotH = max(0.0f, dot(normal, halfVector));
    
    // new specular power calculation
    float normalizationFactor = (power + 8.0f) / (8.0f * 3.14159f);
    float specular = normalizationFactor * pow(NdotH, power);

    // stylized falloff
    float smoothSpec = smoothstep(0.05f, 0.1f, specular);
    specular = lerp(specular, smoothSpec * specular, 0.7f);
    
    return specularColor * specular;
}

// so now we're using that light data struct
// to determine the light contribution based on light type
float3 CalculateDirectionalLight(LightData light, float3 normal, float3 viewDir, float3 baseColor, float roughness)
{
    float3 lightDir = normalize(-light.lightDirection);
    
    // stylized diffuse
    float3 diffuse = CalculateStylizedDiffuse(normal, lightDir, light.diffuseColor.rgb);

    // stylized specular
    // new specular power calculation using roughness
    float specPower = max(1.0f, 64.0f * (1.0f - roughness));
    float3 specular = CalculateStylizedSpecular(normal, lightDir, viewDir, float3(0.3f, 0.3f, 0.3f), specPower);
    
    return (diffuse + specular) * baseColor;
}

float3 CalculatePointLight(LightData light, float3 normal, float3 worldPos, float3 viewDir, float3 baseColor, float roughness)
{
    float3 lightVec = light.lightPosition - worldPos;
    float distance = length(lightVec);
    
    // early out if beyond range with soft falloff
    float rangeCheck = 1.0f - smoothstep(light.lightRange * 0.8f, light.lightRange, distance);
    if (rangeCheck <= 0.0f)
        return float3(0.0f, 0.0f, 0.0f);
    
    float3 lightDir = normalize(lightVec);
    
    // stylized diffuse
    float3 diffuse = CalculateStylizedDiffuse(normal, lightDir, light.diffuseColor.rgb);
    // stylized specular
    float specPower = max(1.0f, 64.0f * (1.0f - roughness));
    float3 specular = CalculateStylizedSpecular(normal, lightDir, viewDir, float3(0.3f, 0.3f, 0.3f), specPower);
    
    // new phys based attenuation with inverse square falloff
    float attenFactor = saturate(1.0f - (distance / light.lightRange));
    float attenuation = attenFactor * attenFactor;

    // subtle distance based color shift
    float3 colorShift = lerp(float3(1.0f, 0.9f, 1.0f), float3(1.0f, 1.0f, 1.0f), attenFactor);
    
    return (diffuse + specular) * baseColor * attenuation * colorShift;
}

float3 CalculateSpotLight(LightData light, float3 normal, float3 worldPos, float3 viewDir, float3 baseColor, float roughness)
{
    float3 lightVec = light.lightPosition - worldPos;
    float distance = length(lightVec);
    
    // early out if beyond range
    if (distance > light.lightRange)
        return float3(0.0f, 0.0f, 0.0f);
    
    float3 lightDir = normalize(lightVec);
    
    // spot cone calculation with stylized falloff
    float spotFactor = dot(-lightDir, normalize(light.lightDirection));
    // soft spotfactor transition between inner and outer cone
    float spotEdge = smoothstep(light.spotOuterCone, lerp(light.spotOuterCone, light.spotInnerCone, 0.5f), spotFactor);
    if (spotEdge <= 0.0f)
        return float3(0.0f, 0.0f, 0.0f);
    
    // stylized diffuse
    float3 diffuse = CalculateStylizedDiffuse(normal, lightDir, light.diffuseColor.rgb);

    // stylized specular
    float specPower = max(1.0f, 64.0f * (1.0f - roughness));
    float3 specular = CalculateStylizedSpecular(normal, lightDir, viewDir, float3(0.3f, 0.3f, 0.3f), specPower);
    
    // distance attenuation
    float attenFactor = saturate(1.0f - (distance / light.lightRange));
    float attenuation = attenFactor * attenFactor;
    
    // spot cone attenuation with improved inner-outer cone falloff
    float spotRatio = smoothstep(light.spotOuterCone, light.spotInnerCone, spotFactor);
    spotRatio = pow(spotRatio, 1.5f); // add more contrast to spotlight edge
    attenuation *= spotRatio;
    
    return (diffuse + specular) * baseColor * attenuation;
}

float4 main(PS_INPUT input) : SV_Target
{
    // sample base color from diffuse texture
    float4 texColor = diffuseTexture.Sample(textureSampler, input.texCoord);
    float3 baseColor = texColor.rgb;

    // you'd extract roughness from an alpha map or smth but i dont have that yet
    // so i'm just gonna use a default value
    float roughness = 0.5f;

    // sample normal map and transform to world space
    float3 normalMapValue = normalTexture.Sample(textureSampler, input.texCoord).rgb;
    normalMapValue = normalize(normalMapValue * 2.0f - 1.0f);
    
    // create orthonormal basis from normal, tangent, and bitangent
    float3 N = normalize(input.normal);
    float3 T = normalize(input.tangent);
    T = normalize(T - dot(T, N) * N); // gram-schmidt orthogonalization
    // cross product to get bitangent
    // completetly forgot about this
    float3 B = normalize(cross(N, T));

    // apply normal map
    float3x3 TBN = float3x3(T, B, N);
    float3 normal = normalize(mul(normalMapValue, TBN));
    
    // get view direction
    float3 viewDir = normalize(cameraPosition - input.worldPos);
    
    // global ambient term
    float3 globalAmbient = float3(0.1f, 0.1f, 0.1f) * baseColor;
    float3 finalColor = globalAmbient;

    // iterate through active lights
    for (int i = 0; i < activeLightCount; i++)
    {
        float3 lightContribution = float3(0, 0, 0);

        // add appropriate light contribution based on light type
        if (lights[i].lightType == LIGHT_DIRECTIONAL)
        {
            lightContribution += CalculateDirectionalLight(lights[i], normal, viewDir, baseColor, roughness);
        }
        else if (lights[i].lightType == LIGHT_POINT)
        {
            lightContribution += CalculatePointLight(lights[i], normal, input.worldPos, viewDir, baseColor, roughness);
        }
        else if (lights[i].lightType == LIGHT_SPOT)
        {
            lightContribution += CalculateSpotLight(lights[i], normal, input.worldPos, viewDir, baseColor, roughness);
        }

        // apply light intensity
        lightContribution *= lights[i].lightIntensity;
        // add to final color
        finalColor += lightContribution;
    }
    
    // add stylized rim lighting
    float3 rimLight = CalculateRimLight(normal, viewDir, float3(0.3f, 0.4f, 0.5f), 3.0f, 0.4f);
    finalColor += rimLight;
    
    // apply a very subtle toon shading by discretizing the final color
    // const float levels = 16.0f;
    // finalColor = floor(finalColor * levels) / levels;

    // optional color grading
    finalColor = saturate(finalColor); // clamp to 0-1 range
    finalColor = finalColor * (1.0f + 0.2f * (finalColor - finalColor * finalColor));

    // optional color temp adjustment
    finalColor = lerp(finalColor, finalColor * float3(1.05f, 1.02f, 0.98f), 0.2f);
    
    return float4(finalColor, texColor.a);
}