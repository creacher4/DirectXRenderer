#pragma once

#include "Component.h"
#include <d3d11.h>
#include <wrl/client.h>
#include <DirectXMath.h>

class MaterialComponent : public Component
{
public:
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> diffuseTexture;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> normalTexture;
    Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;

    DirectX::XMFLOAT4 diffuseColor = {1.0f, 1.0f, 1.0f, 1.0f};
    float specularPower = 32.0f;
};