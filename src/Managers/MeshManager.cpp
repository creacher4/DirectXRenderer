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

            // position
            DirectX::XMFLOAT3 position(
                radius * sinPhi * cosTheta,
                radius * cosPhi,
                radius * sinPhi * sinTheta);

            // position as vector for calculations
            DirectX::XMVECTOR posVec = DirectX::XMLoadFloat3(&position);

            // calculate normal vector
            DirectX::XMVECTOR normalVec = DirectX::XMVector3Normalize(posVec);
            DirectX::XMFLOAT3 normal;
            DirectX::XMStoreFloat3(&normal, normalVec);

            // calculate texture coordinates
            DirectX::XMFLOAT2 texCoord(
                static_cast<float>(slice) / slices,
                static_cast<float>(stack) / stacks);

            // tangent space vectors
            DirectX::XMVECTOR tangentVec;
            DirectX::XMFLOAT3 tangent;

            // handle poles where tangent would be zero
            if (sinPhi < 0.0001f)
            {
                tangentVec = DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
            }
            else
            {
                // calculate tangent vector
                tangent.x = -sinTheta;
                tangent.y = 0;
                tangent.z = cosTheta;

                // normalize tangent
                tangentVec = DirectX::XMLoadFloat3(&tangent);
                tangentVec = DirectX::XMVector3Normalize(tangentVec);
            }

            // calculate bitangent using cross product
            DirectX::XMVECTOR bitangentVec = DirectX::XMVector3Cross(normalVec, tangentVec);
            bitangentVec = DirectX::XMVector3Normalize(bitangentVec);

            // store computed tangent calculations
            DirectX::XMStoreFloat3(&tangent, tangentVec);
            DirectX::XMFLOAT3 bitangent;
            DirectX::XMStoreFloat3(&bitangent, bitangentVec);

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

void MeshManager::CalculateTangentBitangent(
    const DirectX::XMFLOAT3 &v0, const DirectX::XMFLOAT3 &v1, const DirectX::XMFLOAT3 &v2,
    const DirectX::XMFLOAT2 &uv0, const DirectX::XMFLOAT2 &uv1, const DirectX::XMFLOAT2 &uv2,
    DirectX::XMFLOAT3 &tangent, DirectX::XMFLOAT3 &bitangent)
{
    // using SIMD operations instead of manual calculations
    using namespace DirectX;

    // load positions as vectors
    XMVECTOR pos0 = XMLoadFloat3(&v0);
    XMVECTOR pos1 = XMLoadFloat3(&v1);
    XMVECTOR pos2 = XMLoadFloat3(&v2);

    // calculate edges
    XMVECTOR edge1 = XMVectorSubtract(pos1, pos0);
    XMVECTOR edge2 = XMVectorSubtract(pos2, pos0);

    // calculate UV deltas
    float du1 = uv1.x - uv0.x;
    float dv1 = uv1.y - uv0.y;
    float du2 = uv2.x - uv0.x;
    float dv2 = uv2.y - uv0.y;

    // check for degenerate UV mapping
    float determinant = du1 * dv2 - du2 * dv1;
    if (fabs(determinant) < 0.000001f)
    {
        // fallback to default vectors
        tangent = XMFLOAT3(1.0f, 0.0f, 0.0f);
        bitangent = XMFLOAT3(0.0f, 1.0f, 0.0f);
        return;
    }

    float invDet = 1.0f / determinant;

    // calculate tangent and bitanget
    XMVECTOR scaledEdge1 = XMVectorScale(edge1, dv2 * invDet);
    XMVECTOR scaledEdge2 = XMVectorScale(edge2, dv1 * invDet);
    XMVECTOR T = XMVectorSubtract(scaledEdge1, scaledEdge2);

    scaledEdge1 = XMVectorScale(edge2, du1 * invDet);
    scaledEdge2 = XMVectorScale(edge1, du2 * invDet);
    XMVECTOR B = XMVectorSubtract(scaledEdge1, scaledEdge2);

    T = XMVector3Normalize(T);
    B = XMVector3Normalize(B);

    // normalize tangent and bitangent and store
    XMStoreFloat3(&tangent, XMVector3Normalize(T));
    XMStoreFloat3(&bitangent, XMVector3Normalize(B));
}