// documentation has been created for this new lambertian based shader
// just need to update the documentation elsewhere and publish it

// new component system in place which means having to update the shader
// to work with the new system

// next commit maybe

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
    halfLambert = pow(halfLambert, 1.5f); // using pow method for contrast
    
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

float3 CalculateDirectionalLight(float3 normal, float3 viewDir, float3 baseColor, float roughness)
{
    float3 lightDir = normalize(-lightDirection);
    
    // stylized diffuse
    float3 diffuse = CalculateStylizedDiffuse(normal, lightDir, diffuseColor.rgb);
    // stylized specular
    // new specular power calculation using roughness
    float specPower = max(1.0f, 64.0f * (1.0f - roughness));
    float3 specular = CalculateStylizedSpecular(normal, lightDir, viewDir, float3(0.3f, 0.3f, 0.3f), specPower);
    
    return (diffuse + specular) * baseColor;
}

float3 CalculatePointLight(float3 normal, float3 worldPos, float3 viewDir, float3 baseColor, float roughness)
{
    float3 lightVec = lightPosition - worldPos;
    float distance = length(lightVec);
    
    // early out if beyond range with soft falloff
    float rangeCheck = 1.0f - smoothstep(lightRange * 0.8f, lightRange, distance);
    if (rangeCheck <= 0.0f)
        return float3(0.0f, 0.0f, 0.0f);
    
    float3 lightDir = normalize(lightVec);
    
    // stylized diffuse
    float3 diffuse = CalculateStylizedDiffuse(normal, lightDir, diffuseColor.rgb);
    // stylized specular
    float specPower = max(1.0f, 64.0f * (1.0f - roughness));
    float3 specular = CalculateStylizedSpecular(normal, lightDir, viewDir, float3(0.3f, 0.3f, 0.3f), specPower);
    
    // new phys based attenuation with inverse square falloff
    float attenFactor = saturate(1.0f - (distance / lightRange));
    float attenuation = attenFactor * attenFactor;

    // subtle distance based color shift
    float3 colorShift = lerp(float3(1.0f, 0.9f, 1.0f), float3(1.0f, 1.0f, 1.0f), attenFactor);
    
    return (diffuse + specular) * baseColor * attenuation * colorShift;
}

float3 CalculateSpotLight(float3 normal, float3 worldPos, float3 viewDir, float3 baseColor, float roughness)
{
    float3 lightVec = lightPosition - worldPos;
    float distance = length(lightVec);
    
    // early out if beyond range
    if (distance > lightRange)
        return float3(0.0f, 0.0f, 0.0f);
    
    float3 lightDir = normalize(lightVec);
    
    // spot cone calculation with stylized falloff
    float spotFactor = dot(-lightDir, normalize(lightDirection));
    // soft spotfactor transition between inner and outer cone
    float spotEdge = smoothstep(spotOuterCone, lerp(spotOuterCone, spotInnerCone, 0.5f), spotFactor);
    if (spotEdge <= 0.0f)
        return float3(0.0f, 0.0f, 0.0f);
    
    // stylized diffuse
    float3 diffuse = CalculateStylizedDiffuse(normal, lightDir, diffuseColor.rgb);

    // stylized specular
    float specPower = max(1.0f, 64.0f * (1.0f - roughness));
    float3 specular = CalculateStylizedSpecular(normal, lightDir, viewDir, float3(0.3f, 0.3f, 0.3f), specPower);
    
    // distance attenuation
    float attenFactor = saturate(1.0f - (distance / lightRange));
    float attenuation = attenFactor * attenFactor;
    
    // spot cone attenuation with improved inner-outer cone falloff
    float spotRatio = smoothstep(spotOuterCone, spotInnerCone, spotFactor);
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
    
    // start with ambient lighting
    float3 finalColor = ambientColor.rgb * baseColor;
    
    // add appropriate light contribution based on light type
    if (lightType == LIGHT_DIRECTIONAL)
    {
        finalColor += CalculateDirectionalLight(normal, viewDir, baseColor, roughness);
    }
    else if (lightType == LIGHT_POINT)
    {
        finalColor += CalculatePointLight(normal, input.worldPos, viewDir, baseColor, roughness);
    }
    else if (lightType == LIGHT_SPOT)
    {
        finalColor += CalculateSpotLight(normal, input.worldPos, viewDir, baseColor, roughness);
    }
    
    // apply light intensity
    finalColor *= lightIntensity;
    
    // add stylized rim lighting
    float3 rimLight = CalculateRimLight(normal, viewDir, float3(0.3f, 0.4f, 0.5f), 3.0f, 0.4f);
    finalColor += rimLight;
    
    // apply a very subtle toon shading by discretizing the final color
    // comment out this line for a smoother look
    // i have it commented out because i dont like the look of it
    // reminds me of lethal company tbh
    // finalColor = floor(finalColor * 5.0f) / 5.0f;

    // // optional color grading
    // finalColor = saturate(finalColor); // clamp to 0-1 range
    // finalColor = finalColor * (1.0f + 0.2f * (finalColor - finalColor * finalColor));

    // optional tone mapping
    // finalColor = finalColor / (finalColor + 1.0f);

    // optional color temp adjustment
    finalColor = lerp(finalColor, finalColor * float3(1.05f, 1.02f, 0.98f), 0.2f);
    
    return float4(finalColor, texColor.a);
}

// i actually quite like the previous version of the shader
// so im gonna comment it here for reference

// Texture2D diffuseTexture : register(t0);
// Texture2D normalTexture : register(t1);
// SamplerState textureSampler : register(s0);

// // light types
// // might need to fix this because my lights dont work like they should
// // also i really wanna add debug boxes for the lights
// #define LIGHT_DIRECTIONAL 0
// #define LIGHT_POINT 1
// #define LIGHT_SPOT 2

// cbuffer LightBuffer : register(b1)
// {
//     float4 ambientColor;
//     float4 diffuseColor;
//     float3 lightDirection;
//     float padding1;
//     float3 lightPosition;
//     float lightRange;
//     float spotInnerCone;
//     float spotOuterCone;
//     int lightType;
//     float lightIntensity;
// }

// cbuffer CameraBuffer : register(b2)
// {
//     float3 cameraPosition;
//     float padding2;
// }

// struct PS_INPUT
// {
//     float4 position  : SV_POSITION;
//     float4 color     : COLOR;
//     float2 texCoord  : TEXCOORD0;
//     float3 normal    : NORMAL;
//     float3 tangent   : TANGENT;
//     float3 bitangent : BITANGENT;
//     float3 worldPos  : TEXCOORD1;
// };

// // apply fresnel effect for a stylized rim lighting
// float3 CalculateRimLight(float3 normal, float3 viewDir, float3 baseColor, float rimPower)
// {
//     float rim = 1.0f - saturate(dot(normal, viewDir));
//     rim = pow(rim, rimPower);
//     return rim * baseColor * 0.5f;
// }

// // stylized diffuse lighting using half-Lambert
// float3 CalculateStylizedDiffuse(float3 normal, float3 lightDir, float3 lightColor)
// {
//     // half-Lambert diffuse (softer falloff)
//     // talked about in the documentation
//     float NdotL = dot(normal, lightDir) * 0.5f + 0.5f;
//     NdotL = NdotL * NdotL; // contrasty
    
//     return lightColor * NdotL;
// }

// // calculate specular with a stylized falloff
// float3 CalculateStylizedSpecular(float3 normal, float3 lightDir, float3 viewDir, float3 specularColor, float power)
// {
//     float3 halfVector = normalize(lightDir + viewDir);
//     float NdotH = max(0.0f, dot(normal, halfVector));
//     float specular = pow(NdotH, power);
    
//     // apply a smoother falloff
//     specular = smoothstep(0.05f, 0.1f, specular) * specular;
    
//     return specularColor * specular;
// }

// float3 CalculateDirectionalLight(float3 normal, float3 viewDir, float3 baseColor)
// {
//     float3 lightDir = normalize(-lightDirection);
    
//     // stylized diffuse
//     float3 diffuse = CalculateStylizedDiffuse(normal, lightDir, diffuseColor.rgb);
//     // stylized specular
//     float3 specular = CalculateStylizedSpecular(normal, lightDir, viewDir, float3(0.3f, 0.3f, 0.3f), 32.0f);
    
//     return (diffuse + specular) * baseColor;
// }

// float3 CalculatePointLight(float3 normal, float3 worldPos, float3 viewDir, float3 baseColor)
// {
//     float3 lightVec = lightPosition - worldPos;
//     float distance = length(lightVec);
    
//     // early out if beyond range
//     if (distance > lightRange)
//         return float3(0.0f, 0.0f, 0.0f);
    
//     float3 lightDir = normalize(lightVec);
    
//     // stylized diffuse
//     float3 diffuse = CalculateStylizedDiffuse(normal, lightDir, diffuseColor.rgb);
//     // stylized specular
//     float3 specular = CalculateStylizedSpecular(normal, lightDir, viewDir, float3(0.3f, 0.3f, 0.3f), 32.0f);
    
//     // stylized attenuation with smoother falloff
//     float attenFactor = saturate(1.0f - (distance / lightRange));
//     float attenuation = attenFactor * attenFactor;
    
//     return (diffuse + specular) * baseColor * attenuation;
// }

// float3 CalculateSpotLight(float3 normal, float3 worldPos, float3 viewDir, float3 baseColor)
// {
//     float3 lightVec = lightPosition - worldPos;
//     float distance = length(lightVec);
    
//     // early out if beyond range
//     if (distance > lightRange)
//         return float3(0.0f, 0.0f, 0.0f);
    
//     float3 lightDir = normalize(lightVec);
    
//     // spot cone calculation with stylized falloff
//     float spotFactor = dot(-lightDir, normalize(lightDirection));
//     if (spotFactor < spotOuterCone)
//         return float3(0.0f, 0.0f, 0.0f);
    
//     // stylized diffuse
//     float3 diffuse = CalculateStylizedDiffuse(normal, lightDir, diffuseColor.rgb);
//     // stylized specular
//     float3 specular = CalculateStylizedSpecular(normal, lightDir, viewDir, float3(0.3f, 0.3f, 0.3f), 32.0f);
    
//     // distance attenuation
//     float attenFactor = saturate(1.0f - (distance / lightRange));
//     float attenuation = attenFactor * attenFactor;
    
//     // spot attenuation
//     if (spotFactor < spotInnerCone)
//     {
//         float spotRatio = smoothstep(spotOuterCone, spotInnerCone, spotFactor);
//         attenuation *= spotRatio;
//     }
    
//     return (diffuse + specular) * baseColor * attenuation;
// }

// float4 main(PS_INPUT input) : SV_Target
// {
//     // sample normal map and transform to world space
//     float3 normalMapValue = normalTexture.Sample(textureSampler, input.texCoord).rgb;
//     normalMapValue = normalMapValue * 2.0f - 1.0f;
    
//     // create TBN matrix for tangent space to world space conversion
//     float3 N = normalize(input.normal);
//     float3 T = normalize(input.tangent);
//     float3 B = normalize(input.bitangent);
    
//     // ensure tangent is perpendicular to normal
//     T = normalize(T - dot(T, N) * N);
//     B = cross(N, T);
    
//     float3x3 TBN = float3x3(T, B, N);
    
//     // apply normal map
//     float3 normal = normalize(mul(normalMapValue, TBN));
    
//     // get view direction
//     float3 viewDir = normalize(cameraPosition - input.worldPos);
    
//     // sample base color from diffuse texture
//     float4 texColor = diffuseTexture.Sample(textureSampler, input.texCoord);
//     float3 baseColor = texColor.rgb;
    
//     // start with ambient lighting
//     float3 finalColor = ambientColor.rgb * baseColor;
    
//     // add appropriate light contribution based on light type
//     if (lightType == LIGHT_DIRECTIONAL)
//     {
//         finalColor += CalculateDirectionalLight(normal, viewDir, baseColor);
//     }
//     else if (lightType == LIGHT_POINT)
//     {
//         finalColor += CalculatePointLight(normal, input.worldPos, viewDir, baseColor);
//     }
//     else if (lightType == LIGHT_SPOT)
//     {
//         finalColor += CalculateSpotLight(normal, input.worldPos, viewDir, baseColor);
//     }
    
//     // apply light intensity
//     finalColor *= lightIntensity;
    
//     // add stylized rim lighting
//     float3 rimLight = CalculateRimLight(normal, viewDir, float3(0.3f, 0.4f, 0.5f), 3.0f);
//     finalColor += rimLight;
    
//     // apply a very subtle toon shading by discretizing the final color
//     // comment out this line for a smoother look
//     // i have it out commented because i dont like the look of it
//     // reminds me of lethal company tbh
//     // finalColor = floor(finalColor * 5.0f) / 5.0f;
    
//     return float4(finalColor, texColor.a);
// }