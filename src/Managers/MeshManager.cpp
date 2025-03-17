#include "MeshManager.h"

MeshManager::MeshManager(std::shared_ptr<ResourceManager> resourceManager)
    : resourceManager(resourceManager)
{
}

MeshManager::~MeshManager()
{
}

// fixed cube mesh creation
// calculates tangent and bitangent vectors for each face
// based on the actual triangle vertices used to render it
// rather than assuming each group of 4 vertices forms two triangles in a particular order
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

    // indices for the triangles that make up each face
    UINT indices[] = {
        0, 1, 2, 0, 2, 3,       // front face
        4, 5, 6, 4, 6, 7,       // back face
        8, 9, 10, 8, 10, 11,    // left face
        12, 13, 14, 12, 14, 15, // right face
        16, 17, 18, 16, 18, 19, // top face
        20, 21, 22, 20, 22, 23  // bottom face
    };

    // calculate tangents and bitangents for each face
    for (int face = 0; face < 6; face++)
    {
        int baseIndex = face * 4; // each face has 4 vertices
        int triIndex = face * 6;  // each face has 2 triangles (6 indices)

        // calculate tangents and bitangents for first triangle of the face
        DirectX::XMFLOAT3 tangent, bitangent;
        CalculateTangentBitangent(
            tempVertices[indices[triIndex]].position,
            tempVertices[indices[triIndex + 1]].position,
            tempVertices[indices[triIndex + 2]].position,
            tempVertices[indices[triIndex]].texCoord,
            tempVertices[indices[triIndex + 1]].texCoord,
            tempVertices[indices[triIndex + 2]].texCoord,
            tangent, bitangent);

        // assign the same tangent and bitangent to all vertices of this face
        for (int i = 0; i < 4; i++)
        {
            vertices[baseIndex + i].position = tempVertices[baseIndex + i].position;
            vertices[baseIndex + i].normal = tempVertices[baseIndex + i].normal;
            vertices[baseIndex + i].color = tempVertices[baseIndex + i].color;
            vertices[baseIndex + i].texCoord = tempVertices[baseIndex + i].texCoord;
            vertices[baseIndex + i].tangent = tangent;
            vertices[baseIndex + i].bitangent = bitangent;
        }
    }

    MeshData meshData;
    meshData.vertexBuffer = resourceManager->CreateVertexBuffer(vertices, sizeof(vertices));
    meshData.indexBuffer = resourceManager->CreateIndexBuffer(indices, sizeof(indices));
    meshData.indexCount = 36;
    meshData.vertexStride = sizeof(Vertex);

    return meshData;
}

// fixed sphere mesh creation some more
// the indices were generated in the wrong order
// this caused the sphere to render inside out
// also capped the top and bottom of the sphere
// new method provides more accurate tangent space vectors
// based on sphere parameterization
MeshData MeshManager::CreateSphereMesh(float radius, int slices, int stacks)
{
    std::vector<Vertex> vertices;
    std::vector<UINT> indices;

    for (int stack = 0; stack <= stacks; ++stack)
    {
        float phi = stack * DirectX::XM_PI / stacks;
        float sinPhi = sinf(phi);
        float cosPhi = cosf(phi);

        for (int slice = 0; slice <= slices; ++slice)
        {
            float theta = slice * DirectX::XM_2PI / slices;
            float sinTheta = sinf(theta);
            float cosTheta = cosf(theta);

            DirectX::XMFLOAT3 position(
                radius * sinPhi * cosTheta,
                radius * cosPhi,
                radius * sinPhi * sinTheta);

            DirectX::XMFLOAT3 normal(
                position.x / radius,
                position.y / radius,
                position.z / radius);

            DirectX::XMFLOAT2 texCoord(
                static_cast<float>(slice) / slices,
                static_cast<float>(stack) / stacks);

            // calculate proper tangent space vectors
            DirectX::XMFLOAT3 tangent;
            DirectX::XMFLOAT3 bitangent;

            // tangent along theta direction (around the sphere)
            tangent.x = -radius * sinPhi * sinTheta;
            tangent.y = 0;
            tangent.z = radius * sinPhi * cosTheta;

            // handle poles where tangent would be zero
            if (sinPhi < 0.0001f)
            {
                tangent = DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f);
            }
            else
            {
                // normalize tangent
                float len = sqrtf(tangent.x * tangent.x + tangent.y * tangent.y + tangent.z * tangent.z);
                tangent.x /= len;
                tangent.y /= len;
                tangent.z /= len;
            }

            // calculate bitangent using cross product: B = N × T
            bitangent.x = (normal.y * tangent.z) - (normal.z * tangent.y);
            bitangent.y = (normal.z * tangent.x) - (normal.x * tangent.z);
            bitangent.z = (normal.x * tangent.y) - (normal.y * tangent.x);

            // normalize bitangent
            float len = sqrtf(bitangent.x * bitangent.x + bitangent.y * bitangent.y + bitangent.z * bitangent.z);
            bitangent.x /= len;
            bitangent.y /= len;
            bitangent.z /= len;

            Vertex vertex;
            vertex.position = position;
            vertex.normal = normal;
            vertex.texCoord = texCoord;
            vertex.color = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
            vertex.tangent = tangent;
            vertex.bitangent = bitangent;

            vertices.push_back(vertex);
        }
    }

    for (int stack = 0; stack < stacks; ++stack)
    {
        for (int slice = 0; slice < slices; ++slice)
        {
            int current = stack * (slices + 1) + slice;
            int next = current + 1;
            int bottom = current + (slices + 1);
            int bottomNext = bottom + 1;

            // i knew there was something wrong with this
            // reversed index generation
            // sphere mesh renders properly now
            if (stack > 0 || stack < stacks - 1)
            {
                indices.push_back(current);
                indices.push_back(next);
                indices.push_back(bottom);
            }

            if (stack < stacks - 1)
            {
                indices.push_back(next);
                indices.push_back(bottomNext);
                indices.push_back(bottom);
            }
        }
    }

    MeshData meshData;
    meshData.vertexBuffer = resourceManager->CreateVertexBuffer(vertices.data(), static_cast<UINT>(vertices.size() * sizeof(Vertex)));
    meshData.indexBuffer = resourceManager->CreateIndexBuffer(indices.data(), static_cast<UINT>(indices.size() * sizeof(UINT)));
    meshData.indexCount = static_cast<UINT>(indices.size());
    meshData.vertexStride = sizeof(Vertex);

    return meshData;
}

// works
// but instead of texturing, i need to create a procedural grid
// beginnings of a compute shader?
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

// added a check for division by zero
// if the denominator is too small, fallback to default tangent and bitangent
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

    float denominator = (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

    // check for division by zero
    if (abs(denominator) < 0.000001f)
    {
        // fallback to default tangent and bitangent
        tangent = XMFLOAT3(1.0f, 0.0f, 0.0f);
        bitangent = XMFLOAT3(0.0f, 1.0f, 0.0f);
        return;
    }

    float f = 1.0f / denominator;

    tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
    tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
    tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

    bitangent.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
    bitangent.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
    bitangent.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);

    float tangentLength = sqrtf(tangent.x * tangent.x + tangent.y * tangent.y + tangent.z * tangent.z);
    if (tangentLength > 0.00001f)
    {
        tangent.x /= tangentLength;
        tangent.y /= tangentLength;
        tangent.z /= tangentLength;
    }

    float bitangentLength = sqrtf(bitangent.x * bitangent.x + bitangent.y * bitangent.y + bitangent.z * bitangent.z);
    if (bitangentLength > 0.00001f)
    {
        bitangent.x /= bitangentLength;
        bitangent.y /= bitangentLength;
        bitangent.z /= bitangentLength;
    }
}