#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include <DirectXMath.h>
#include <memory>
#include "../Components/LightComponent.h"
#include "../Registry.h"
#include "../Buffers.h"

class LightingManager
{
public:
    LightingManager(Microsoft::WRL::ComPtr<ID3D11Device> device,
                    Microsoft::WRL::ComPtr<ID3D11DeviceContext> context,
                    Registry &registry);
    ~LightingManager();

    bool Initialize();
    void Update();
    void Apply(UINT slot);

private:
    Microsoft::WRL::ComPtr<ID3D11Device> device;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;
    Registry &registry;
    Microsoft::WRL::ComPtr<ID3D11Buffer> lightConstantBuffer;
};