#pragma once

#include <DirectXMath.h>

// some small number for now
#define MAX_LIGHTS 8

struct MatrixBuffer
{
    DirectX::XMMATRIX world;
    DirectX::XMMATRIX wvp;
};

// this is the buffer that will be used to pass the light data to the pixel shader
// so we can just change the name of this buffer to LightData instead
struct LightData
{
    DirectX::XMFLOAT4 ambientColor;
    DirectX::XMFLOAT4 diffuseColor;
    DirectX::XMFLOAT3 lightDirection;
    float padding1;

    DirectX::XMFLOAT3 lightPosition;
    float lightRange;

    float spotInnerCone;
    float spotOuterCone;

    int lightType;
    float lightIntensity;
};

struct LightBuffer
{
    LightData lights[MAX_LIGHTS];
    int activeLightCount;
    DirectX::XMFLOAT3 padding;
};

struct CameraBuffer
{
    DirectX::XMFLOAT3 cameraPosition;
    float padding;
};