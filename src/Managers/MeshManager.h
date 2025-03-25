#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include <DirectXMath.h>
#include <vector>
#include <memory>
#include "../Vertex.h"
#include "ResourceManager.h"

struct MeshData
{
    Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
    UINT indexCount;
    UINT vertexStride;
};

class MeshManager
{
public:
    MeshManager(std::shared_ptr<ResourceManager> resourceManager);
    ~MeshManager() = default;

    MeshData CreateCubeMesh();
    MeshData CreateSphereMesh(float radius, int slices, int stacks);
    MeshData CreatePlaneMesh(float width, float depth, int xDivs, int zDivs);

    void CalculateTangentBitangent(
        const DirectX::XMFLOAT3 &v0, const DirectX::XMFLOAT3 &v1, const DirectX::XMFLOAT3 &v2,
        const DirectX::XMFLOAT2 &uv0, const DirectX::XMFLOAT2 &uv1, const DirectX::XMFLOAT2 &uv2,
        DirectX::XMFLOAT3 &tangent, DirectX::XMFLOAT3 &bitangent);

private:
    std::shared_ptr<ResourceManager> resourceManager;
};