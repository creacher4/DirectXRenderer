#include "MeshManager.h"

MeshManager::MeshManager(std::shared_ptr<ResourceManager> resourceManager)
    : resourceManager(resourceManager)
{
}

MeshManager::~MeshManager()
{
}

MeshData MeshManager::CreateCubeMesh()
{
    struct TempVertex
    {
        DirectX::XMFLOAT3 position;
        DirectX::XMFLOAT3 normal;
        DirectX::XMFLOAT4 color;
        DirectX::XMFLOAT2 texCoord;
    };

    TempVertex tempVertices[] = {
        // front face (normal: 0, 0, -1)
        {DirectX::XMFLOAT3(-0.5f, 0.5f, -0.5f), DirectX::XMFLOAT3(0, 0, -1), DirectX::XMFLOAT4(1, 1, 1, 1), DirectX::XMFLOAT2(0, 0)},
        {DirectX::XMFLOAT3(0.5f, 0.5f, -0.5f), DirectX::XMFLOAT3(0, 0, -1), DirectX::XMFLOAT4(1, 1, 1, 1), DirectX::XMFLOAT2(1, 0)},
        {DirectX::XMFLOAT3(0.5f, -0.5f, -0.5f), DirectX::XMFLOAT3(0, 0, -1), DirectX::XMFLOAT4(1, 1, 1, 1), DirectX::XMFLOAT2(1, 1)},
        {DirectX::XMFLOAT3(-0.5f, -0.5f, -0.5f), DirectX::XMFLOAT3(0, 0, -1), DirectX::XMFLOAT4(1, 1, 1, 1), DirectX::XMFLOAT2(0, 1)},

        // back face (normal: 0, 0, 1)
        {DirectX::XMFLOAT3(0.5f, 0.5f, 0.5f), DirectX::XMFLOAT3(0, 0, 1), DirectX::XMFLOAT4(1, 1, 1, 1), DirectX::XMFLOAT2(0, 0)},
        {DirectX::XMFLOAT3(-0.5f, 0.5f, 0.5f), DirectX::XMFLOAT3(0, 0, 1), DirectX::XMFLOAT4(1, 1, 1, 1), DirectX::XMFLOAT2(1, 0)},
        {DirectX::XMFLOAT3(-0.5f, -0.5f, 0.5f), DirectX::XMFLOAT3(0, 0, 1), DirectX::XMFLOAT4(1, 1, 1, 1), DirectX::XMFLOAT2(1, 1)},
        {DirectX::XMFLOAT3(0.5f, -0.5f, 0.5f), DirectX::XMFLOAT3(0, 0, 1), DirectX::XMFLOAT4(1, 1, 1, 1), DirectX::XMFLOAT2(0, 1)},

        // left face (normal: -1, 0, 0)
        {DirectX::XMFLOAT3(-0.5f, 0.5f, 0.5f), DirectX::XMFLOAT3(-1, 0, 0), DirectX::XMFLOAT4(1, 1, 1, 1), DirectX::XMFLOAT2(0, 0)},
        {DirectX::XMFLOAT3(-0.5f, 0.5f, -0.5f), DirectX::XMFLOAT3(-1, 0, 0), DirectX::XMFLOAT4(1, 1, 1, 1), DirectX::XMFLOAT2(1, 0)},
        {DirectX::XMFLOAT3(-0.5f, -0.5f, -0.5f), DirectX::XMFLOAT3(-1, 0, 0), DirectX::XMFLOAT4(1, 1, 1, 1), DirectX::XMFLOAT2(1, 1)},
        {DirectX::XMFLOAT3(-0.5f, -0.5f, 0.5f), DirectX::XMFLOAT3(-1, 0, 0), DirectX::XMFLOAT4(1, 1, 1, 1), DirectX::XMFLOAT2(0, 1)},

        // right face (normal: 1, 0, 0)
        {DirectX::XMFLOAT3(0.5f, 0.5f, -0.5f), DirectX::XMFLOAT3(1, 0, 0), DirectX::XMFLOAT4(1, 1, 1, 1), DirectX::XMFLOAT2(0, 0)},
        {DirectX::XMFLOAT3(0.5f, 0.5f, 0.5f), DirectX::XMFLOAT3(1, 0, 0), DirectX::XMFLOAT4(1, 1, 1, 1), DirectX::XMFLOAT2(1, 0)},
        {DirectX::XMFLOAT3(0.5f, -0.5f, 0.5f), DirectX::XMFLOAT3(1, 0, 0), DirectX::XMFLOAT4(1, 1, 1, 1), DirectX::XMFLOAT2(1, 1)},
        {DirectX::XMFLOAT3(0.5f, -0.5f, -0.5f), DirectX::XMFLOAT3(1, 0, 0), DirectX::XMFLOAT4(1, 1, 1, 1), DirectX::XMFLOAT2(0, 1)},

        // top face (normal: 0, 1, 0)
        {DirectX::XMFLOAT3(-0.5f, 0.5f, 0.5f), DirectX::XMFLOAT3(0, 1, 0), DirectX::XMFLOAT4(1, 1, 1, 1), DirectX::XMFLOAT2(0, 0)},
        {DirectX::XMFLOAT3(0.5f, 0.5f, 0.5f), DirectX::XMFLOAT3(0, 1, 0), DirectX::XMFLOAT4(1, 1, 1, 1), DirectX::XMFLOAT2(1, 0)},
        {DirectX::XMFLOAT3(0.5f, 0.5f, -0.5f), DirectX::XMFLOAT3(0, 1, 0), DirectX::XMFLOAT4(1, 1, 1, 1), DirectX::XMFLOAT2(1, 1)},
        {DirectX::XMFLOAT3(-0.5f, 0.5f, -0.5f), DirectX::XMFLOAT3(0, 1, 0), DirectX::XMFLOAT4(1, 1, 1, 1), DirectX::XMFLOAT2(0, 1)},

        // bottom face (normal: 0, -1, 0)
        {DirectX::XMFLOAT3(-0.5f, -0.5f, -0.5f), DirectX::XMFLOAT3(0, -1, 0), DirectX::XMFLOAT4(1, 1, 1, 1), DirectX::XMFLOAT2(0, 0)},
        {DirectX::XMFLOAT3(0.5f, -0.5f, -0.5f), DirectX::XMFLOAT3(0, -1, 0), DirectX::XMFLOAT4(1, 1, 1, 1), DirectX::XMFLOAT2(1, 0)},
        {DirectX::XMFLOAT3(0.5f, -0.5f, 0.5f), DirectX::XMFLOAT3(0, -1, 0), DirectX::XMFLOAT4(1, 1, 1, 1), DirectX::XMFLOAT2(1, 1)},
        {DirectX::XMFLOAT3(-0.5f, -0.5f, 0.5f), DirectX::XMFLOAT3(0, -1, 0), DirectX::XMFLOAT4(1, 1, 1, 1), DirectX::XMFLOAT2(0, 1)},
    };

    Vertex vertices[24];
    DirectX::XMFLOAT3 tangent, bitangent;

    // front face (triangles 0-1)
    CalculateTangentBitangent(
        tempVertices[0].position, tempVertices[1].position, tempVertices[2].position,
        tempVertices[0].texCoord, tempVertices[1].texCoord, tempVertices[2].texCoord,
        tangent, bitangent);

    for (int i = 0; i < 4; i++)
    {
        vertices[i].position = tempVertices[i].position;
        vertices[i].normal = tempVertices[i].normal;
        vertices[i].color = tempVertices[i].color;
        vertices[i].texCoord = tempVertices[i].texCoord;
        vertices[i].tangent = tangent;
        vertices[i].bitangent = bitangent;
    }

    // back face (triangles 4-5)
    CalculateTangentBitangent(
        tempVertices[4].position, tempVertices[5].position, tempVertices[6].position,
        tempVertices[4].texCoord, tempVertices[5].texCoord, tempVertices[6].texCoord,
        tangent, bitangent);

    for (int i = 4; i < 8; i++)
    {
        vertices[i].position = tempVertices[i].position;
        vertices[i].normal = tempVertices[i].normal;
        vertices[i].color = tempVertices[i].color;
        vertices[i].texCoord = tempVertices[i].texCoord;
        vertices[i].tangent = tangent;
        vertices[i].bitangent = bitangent;
    }

    // left face (triangles 8-9)
    CalculateTangentBitangent(
        tempVertices[8].position, tempVertices[9].position, tempVertices[10].position,
        tempVertices[8].texCoord, tempVertices[9].texCoord, tempVertices[10].texCoord,
        tangent, bitangent);

    for (int i = 8; i < 12; i++)
    {
        vertices[i].position = tempVertices[i].position;
        vertices[i].normal = tempVertices[i].normal;
        vertices[i].color = tempVertices[i].color;
        vertices[i].texCoord = tempVertices[i].texCoord;
        vertices[i].tangent = tangent;
        vertices[i].bitangent = bitangent;
    }

    // right face (triangles 12-13)
    CalculateTangentBitangent(
        tempVertices[12].position, tempVertices[13].position, tempVertices[14].position,
        tempVertices[12].texCoord, tempVertices[13].texCoord, tempVertices[14].texCoord,
        tangent, bitangent);

    for (int i = 12; i < 16; i++)
    {
        vertices[i].position = tempVertices[i].position;
        vertices[i].normal = tempVertices[i].normal;
        vertices[i].color = tempVertices[i].color;
        vertices[i].texCoord = tempVertices[i].texCoord;
        vertices[i].tangent = tangent;
        vertices[i].bitangent = bitangent;
    }

    // top face (triangles 16-17)
    CalculateTangentBitangent(
        tempVertices[16].position, tempVertices[17].position, tempVertices[18].position,
        tempVertices[16].texCoord, tempVertices[17].texCoord, tempVertices[18].texCoord,
        tangent, bitangent);

    for (int i = 16; i < 20; i++)
    {
        vertices[i].position = tempVertices[i].position;
        vertices[i].normal = tempVertices[i].normal;
        vertices[i].color = tempVertices[i].color;
        vertices[i].texCoord = tempVertices[i].texCoord;
        vertices[i].tangent = tangent;
        vertices[i].bitangent = bitangent;
    }

    // bottom face (triangles 20-21)
    CalculateTangentBitangent(
        tempVertices[20].position, tempVertices[21].position, tempVertices[22].position,
        tempVertices[20].texCoord, tempVertices[21].texCoord, tempVertices[22].texCoord,
        tangent, bitangent);

    for (int i = 20; i < 24; i++)
    {
        vertices[i].position = tempVertices[i].position;
        vertices[i].normal = tempVertices[i].normal;
        vertices[i].color = tempVertices[i].color;
        vertices[i].texCoord = tempVertices[i].texCoord;
        vertices[i].tangent = tangent;
        vertices[i].bitangent = bitangent;
    }

    UINT indices[] = {
        0, 1, 2, 0, 2, 3,       // front face
        4, 5, 6, 4, 6, 7,       // back face
        8, 9, 10, 8, 10, 11,    // left face
        12, 13, 14, 12, 14, 15, // right face
        16, 17, 18, 16, 18, 19, // top face
        20, 21, 22, 20, 22, 23  // bottom face
    };

    MeshData meshData;
    meshData.vertexBuffer = resourceManager->CreateVertexBuffer(vertices, sizeof(vertices));
    meshData.indexBuffer = resourceManager->CreateIndexBuffer(indices, sizeof(indices));
    meshData.indexCount = 36;
    meshData.vertexStride = sizeof(Vertex);

    return meshData;
}

// need to recreate this function
// idk what the hell this function does right now but when loading the sphere mesh
// it acts weirdly
// idk it just doesn't work
MeshData MeshManager::CreateSphereMesh(float radius, int slices, int stacks)
{
    std::vector<Vertex> vertices;
    std::vector<UINT> indices;

    // Create the vertices
    for (int stack = 0; stack <= stacks; stack++)
    {
        float phi = DirectX::XM_PI * float(stack) / float(stacks);
        float y = radius * cosf(phi);
        float stackRadius = radius * sinf(phi);

        for (int slice = 0; slice <= slices; slice++)
        {
            float theta = 2.0f * DirectX::XM_PI * float(slice) / float(slices);
            float x = stackRadius * cosf(theta);
            float z = stackRadius * sinf(theta);

            DirectX::XMFLOAT3 position = {x, y, z};
            DirectX::XMFLOAT3 normal = {x / radius, y / radius, z / radius};
            DirectX::XMFLOAT2 texCoord = {float(slice) / slices, float(stack) / stacks};
            DirectX::XMFLOAT4 color = {1.0f, 1.0f, 1.0f, 1.0f};

            DirectX::XMFLOAT3 tangent = {-z, 0.0f, x};
            float tangentLength = sqrtf(tangent.x * tangent.x + tangent.y * tangent.y + tangent.z * tangent.z);
            if (tangentLength > 0.0f)
            {
                tangent.x /= tangentLength;
                tangent.y /= tangentLength;
                tangent.z /= tangentLength;
            }

            DirectX::XMVECTOR normalVec = DirectX::XMLoadFloat3(&normal);
            DirectX::XMVECTOR tangentVec = DirectX::XMLoadFloat3(&tangent);
            DirectX::XMVECTOR bitangentVec = DirectX::XMVector3Cross(normalVec, tangentVec);

            DirectX::XMFLOAT3 bitangent;
            DirectX::XMStoreFloat3(&bitangent, bitangentVec);

            Vertex vertex;
            vertex.position = position;
            vertex.normal = normal;
            vertex.color = color;
            vertex.texCoord = texCoord;
            vertex.tangent = tangent;
            vertex.bitangent = bitangent;

            vertices.push_back(vertex);
        }
    }

    for (int stack = 0; stack < stacks; stack++)
    {
        for (int slice = 0; slice < slices; slice++)
        {
            int topLeft = stack * (slices + 1) + slice;
            int topRight = topLeft + 1;
            int bottomLeft = (stack + 1) * (slices + 1) + slice;
            int bottomRight = bottomLeft + 1;

            indices.push_back(topLeft);
            indices.push_back(bottomLeft);
            indices.push_back(topRight);

            indices.push_back(topRight);
            indices.push_back(bottomLeft);
            indices.push_back(bottomRight);
        }
    }

    MeshData meshData;
    meshData.vertexBuffer = resourceManager->CreateVertexBuffer(vertices.data(), static_cast<UINT>(vertices.size() * sizeof(Vertex)));
    meshData.indexBuffer = resourceManager->CreateIndexBuffer(indices.data(), static_cast<UINT>(indices.size() * sizeof(UINT)));
    meshData.indexCount = static_cast<UINT>(indices.size());
    meshData.vertexStride = sizeof(Vertex);

    return meshData;
}

// this does work
// but im yet to test it properly
MeshData MeshManager::CreatePlaneMesh(float width, float depth, int xDivs, int zDivs)
{
    std::vector<Vertex> vertices;
    std::vector<UINT> indices;

    float halfWidth = width / 2.0f;
    float halfDepth = depth / 2.0f;
    float xStep = width / xDivs;
    float zStep = depth / zDivs;
    float uStep = 1.0f / xDivs;
    float vStep = 1.0f / zDivs;

    for (int z = 0; z <= zDivs; ++z)
    {
        float zPos = -halfDepth + z * zStep;
        float v = z * vStep;

        for (int x = 0; x <= xDivs; ++x)
        {
            float xPos = -halfWidth + x * xStep;
            float u = x * uStep;

            Vertex vertex;
            vertex.position = DirectX::XMFLOAT3(xPos, 0.0f, zPos);
            vertex.normal = DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f);
            vertex.texCoord = DirectX::XMFLOAT2(u, v);
            vertex.color = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
            vertex.tangent = DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f);
            vertex.bitangent = DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f);

            vertices.push_back(vertex);
        }
    }

    for (int z = 0; z < zDivs; ++z)
    {
        for (int x = 0; x < xDivs; ++x)
        {
            int topLeft = z * (xDivs + 1) + x;
            int topRight = topLeft + 1;
            int bottomLeft = (z + 1) * (xDivs + 1) + x;
            int bottomRight = bottomLeft + 1;

            indices.push_back(topLeft);
            indices.push_back(bottomLeft);
            indices.push_back(topRight);

            indices.push_back(topRight);
            indices.push_back(bottomLeft);
            indices.push_back(bottomRight);
        }
    }

    MeshData meshData;
    meshData.vertexBuffer = resourceManager->CreateVertexBuffer(vertices.data(), static_cast<UINT>(vertices.size() * sizeof(Vertex)));
    meshData.indexBuffer = resourceManager->CreateIndexBuffer(indices.data(), static_cast<UINT>(indices.size() * sizeof(UINT)));
    meshData.indexCount = static_cast<UINT>(indices.size());
    meshData.vertexStride = sizeof(Vertex);

    return meshData;
}

void MeshManager::CalculateTangentBitangent(
    const DirectX::XMFLOAT3 &v0, const DirectX::XMFLOAT3 &v1, const DirectX::XMFLOAT3 &v2,
    const DirectX::XMFLOAT2 &uv0, const DirectX::XMFLOAT2 &uv1, const DirectX::XMFLOAT2 &uv2,
    DirectX::XMFLOAT3 &tangent, DirectX::XMFLOAT3 &bitangent)
{
    using namespace DirectX;

    XMFLOAT3 edge1(v1.x - v0.x, v1.y - v0.y, v1.z - v0.z);
    XMFLOAT3 edge2(v2.x - v0.x, v2.y - v0.y, v2.z - v0.z);

    XMFLOAT2 deltaUV1(uv1.x - uv0.x, uv1.y - uv0.y);
    XMFLOAT2 deltaUV2(uv2.x - uv0.x, uv2.y - uv0.y);

    float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

    tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
    tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
    tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

    bitangent.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
    bitangent.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
    bitangent.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);

    float tangentLength = sqrtf(tangent.x * tangent.x + tangent.y * tangent.y + tangent.z * tangent.z);
    tangent.x /= tangentLength;
    tangent.y /= tangentLength;
    tangent.z /= tangentLength;

    float bitangentLength = sqrtf(bitangent.x * bitangent.x + bitangent.y * bitangent.y + bitangent.z * bitangent.z);
    bitangent.x /= bitangentLength;
    bitangent.y /= bitangentLength;
    bitangent.z /= bitangentLength;
}