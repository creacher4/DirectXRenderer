#pragma once

#include <DirectXMath.h>

// honestly, need to fix the layouts and names of these buffers

struct MatrixBuffer
{
    DirectX::XMMATRIX world;
    DirectX::XMMATRIX wvp;
};

struct LightBuffer
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

struct CameraBuffer
{
    DirectX::XMFLOAT3 cameraPosition;
    float padding;
};