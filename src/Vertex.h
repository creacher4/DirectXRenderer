#pragma once

#include <DirectXMath.h>

struct Vertex
{
    DirectX::XMFLOAT3 position;
    DirectX::XMFLOAT3 normal;
    DirectX::XMFLOAT4 color;
    DirectX::XMFLOAT2 texCoord;
    DirectX::XMFLOAT3 tangent;
    DirectX::XMFLOAT3 bitangent;
};
