#pragma once

#include "../Component.h"
#include <d3d11.h>
#include <wrl/client.h>

class MeshComponent : public Component
{
public:
    Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
    UINT indexCount = 0;
    UINT vertexStride = 0;
};