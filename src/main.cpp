#define UNICODE
#define _UNICODE

#include <windows.h>
#include <d3d11.h>
#include <dxgi.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <iostream>
#include <fstream>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

using namespace DirectX;

// window properties
LPCWSTR WndClassName = L"DX11Window";
HWND hwnd = NULL;
const int Width = 800;
const int Height = 600;

// direct3D objects (necessary)
IDXGISwapChain *swapChain = nullptr;
ID3D11Device *d3d11Device = nullptr;
ID3D11DeviceContext *d3d11DevCon = nullptr;
ID3D11RenderTargetView *renderTargetView = nullptr;
ID3D11Buffer *vertexBuffer = nullptr;       // can remove this if only using squareVertexBuffer
ID3D11Buffer *squareVertexBuffer = nullptr; // new vertex buffers for the square
ID3D11Buffer *squareIndexBuffer = nullptr;  // new index buffer for the square
ID3D11InputLayout *inputLayout = nullptr;
ID3D11VertexShader *vertexShader = nullptr;
ID3D11PixelShader *pixelShader = nullptr;
ID3D11Texture2D *depthStencilBuffer = nullptr;
ID3D11DepthStencilView *depthStencilView = nullptr;

// matrices for camera and world

XMMATRIX WVP;           // world-view-projection matrix used in vertex shader
XMMATRIX World;         // world transformation matrix
XMMATRIX camView;       // camera view matrix
XMMATRIX camProjection; // projection matrix

// vectors for camera
XMVECTOR camPosition; // camera position
XMVECTOR camTarget;   // camera target (where it's looking)
XMVECTOR camUp;       // up direction

// world matrices for cubes
XMMATRIX cube1World;
XMMATRIX cube2World;

XMMATRIX Translation1;
XMMATRIX Translation2;

// camera movement parameters
float camYaw = 0.0f;
float camPitch = 0.0f;
float moveSpeed = 0.1f;
float turnSpeed = 0.01f;

// constant buffer for camera
struct cbPerObject
{
    XMMATRIX WVP; // world-view-projection matrix
};

// create constant buffer for camera
ID3D11Buffer *cbPerObjectBuffer;
cbPerObject cbPerObj;

// function declarations
bool InitializeWindow(HINSTANCE hInstance, int ShowWnd, int width, int height, bool windowed);
bool InitializeDirect3D();
bool LoadShaders();
void UpdateScene();
void DrawScene();
void ReleaseObjects();
int MessageLoop();
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// vertex structure
struct Vertex
{
    XMFLOAT3 pos;
    XMFLOAT4 color;
};

// cube vertex data
Vertex cubeVertices[] =
    {
        // front face
        {XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f)}, // red
        {XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f)},  // green
        {XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f)},   // blue
        {XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f)},  // yellow

        // back face
        {XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f)}, // purple
        {XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f)},  // cyan
        {XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)},   // white
        {XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f)}   // gray
};

// cube index data
DWORD cubeIndices[] =
    {
        // front face
        0, 1, 2, 0, 2, 3,

        // back face
        4, 6, 5, 4, 7, 6,

        // left face
        4, 5, 1, 4, 1, 0,

        // right face
        3, 2, 6, 3, 6, 7,

        // top face
        1, 5, 6, 1, 6, 2,

        // bottom face
        4, 0, 3, 4, 3, 7};

// shader loading helper function
HRESULT LoadCompiledShader(LPCWSTR filename, ID3DBlob **shaderBlob)
{
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (!file.is_open())
        return E_FAIL;

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    *shaderBlob = nullptr;
    D3DCreateBlob(size, shaderBlob);
    if (!*shaderBlob)
        return E_FAIL;

    file.read((char *)(*shaderBlob)->GetBufferPointer(), size);
    file.close();
    return S_OK;
}

// WINMAIN: entry point of the application
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    if (!InitializeWindow(hInstance, nShowCmd, Width, Height, true))
    {
        MessageBoxW(0, L"Window Initialization Failed", L"Error", MB_OK);
        return 0;
    }

    if (!InitializeDirect3D())
    {
        MessageBoxW(0, L"Direct3D Initialization Failed", L"Error", MB_OK);
        return 0;
    }

    if (!LoadShaders())
    {
        MessageBoxW(0, L"Shader Compilation Failed", L"Error", MB_OK);
        return 0;
    }

    MSG msg = {};
    while (msg.message != WM_QUIT)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            UpdateScene();
            DrawScene();
        }
    }

    ReleaseObjects();
    return (int)msg.wParam;
}

// load and create shaders
bool LoadShaders()
{
    ID3DBlob *vsBlob = nullptr;
    ID3DBlob *psBlob = nullptr;

    if (FAILED(LoadCompiledShader(L"shaders/VertexShader.cso", &vsBlob)))
    {
        MessageBoxW(0, L"Failed to load Vertex Shader (Missing or Corrupt)", L"Error", MB_OK);
        return false;
    }

    if (FAILED(LoadCompiledShader(L"shaders/PixelShader.cso", &psBlob)))
    {
        MessageBoxW(0, L"Failed to load Pixel Shader (Missing or Corrupt)", L"Error", MB_OK);
        return false;
    }

    HRESULT hr = d3d11Device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &vertexShader);
    if (FAILED(hr))
    {
        MessageBoxW(0, L"Failed to create Vertex Shader", L"Error", MB_OK);
        return false;
    }

    hr = d3d11Device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &pixelShader);
    if (FAILED(hr))
    {
        MessageBoxW(0, L"Failed to create Pixel Shader", L"Error", MB_OK);
        return false;
    }

    d3d11DevCon->VSSetShader(vertexShader, 0, 0);
    d3d11DevCon->PSSetShader(pixelShader, 0, 0);

    // define input layout (vertex position & colour)
    D3D11_INPUT_ELEMENT_DESC layout[] =
        {
            {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
        };
    UINT numElements = ARRAYSIZE(layout);

    // create input layout
    hr = d3d11Device->CreateInputLayout(layout, numElements, vsBlob->GetBufferPointer(),
                                        vsBlob->GetBufferSize(), &inputLayout);
    if (FAILED(hr))
    {
        MessageBoxW(0, L"Failed to create Input Layout", L"Error", MB_OK);
        return false;
    }

    d3d11DevCon->IASetInputLayout(inputLayout);

    vsBlob->Release();
    psBlob->Release();

    return true;
}

// create a cube (duh)
bool CreateCube()
{
    // create the vertex Buffer
    D3D11_BUFFER_DESC vertexBufferDesc = {};
    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = sizeof(cubeVertices);
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA vertexData = {};
    vertexData.pSysMem = cubeVertices;

    HRESULT hr = d3d11Device->CreateBuffer(&vertexBufferDesc, &vertexData, &squareVertexBuffer);
    if (FAILED(hr))
    {
        MessageBox(0, L"Failed to create vertex buffer", L"Error", MB_OK);
        return false;
    }

    // create an index buffer
    D3D11_BUFFER_DESC indexBufferDesc = {};
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(cubeIndices);
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA indexData = {};
    indexData.pSysMem = cubeIndices;

    hr = d3d11Device->CreateBuffer(&indexBufferDesc, &indexData, &squareIndexBuffer);
    if (FAILED(hr))
    {
        MessageBox(0, L"Failed to create index buffer", L"Error", MB_OK);
        return false;
    }

    return true;
}

// initialize and create window
bool InitializeWindow(HINSTANCE hInstance, int ShowWnd, int width, int height, bool windowed)
{
    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = WndClassName;

    if (!RegisterClassExW(&wc))
    {
        MessageBoxW(NULL, L"Error registering class", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }

    hwnd = CreateWindowExW(
        NULL, WndClassName, L"DirectX 11 Window",
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
        width, height, NULL, NULL, hInstance, NULL);

    if (!hwnd)
    {
        MessageBoxW(NULL, L"Error creating window", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }

    ShowWindow(hwnd, ShowWnd);
    UpdateWindow(hwnd);
    return true;
}

// initialize Direct3D
bool InitializeDirect3D()
{
    DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
    swapChainDesc.BufferDesc.Width = Width;
    swapChainDesc.BufferDesc.Height = Height;
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.BufferCount = 1;
    swapChainDesc.OutputWindow = hwnd;
    swapChainDesc.Windowed = true;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    HRESULT hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0,
                                               nullptr, 0, D3D11_SDK_VERSION, &swapChainDesc, &swapChain, &d3d11Device, nullptr, &d3d11DevCon);
    if (FAILED(hr))
        return false;

    // CREATE A BACK BUFFER FOR RENDERING
    ID3D11Texture2D *backBuffer;
    hr = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void **)&backBuffer);
    if (FAILED(hr))
        return false;

    hr = d3d11Device->CreateRenderTargetView(backBuffer, nullptr, &renderTargetView);
    backBuffer->Release();
    if (FAILED(hr))
        return false;

    // CREATE A DEPTH/STENCIL BUFFER FOR DEPTH TESTING
    D3D11_TEXTURE2D_DESC depthStencilDesc = {};
    depthStencilDesc.Width = Width;
    depthStencilDesc.Height = Height;
    depthStencilDesc.MipLevels = 1;
    depthStencilDesc.ArraySize = 1;
    depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthStencilDesc.SampleDesc.Count = 1;
    depthStencilDesc.SampleDesc.Quality = 0;
    depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
    depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depthStencilDesc.CPUAccessFlags = 0;
    depthStencilDesc.MiscFlags = 0;

    hr = d3d11Device->CreateTexture2D(&depthStencilDesc, nullptr, &depthStencilBuffer);
    if (FAILED(hr))
        return false;

    hr = d3d11Device->CreateDepthStencilView(depthStencilBuffer, nullptr, &depthStencilView);
    if (FAILED(hr))
        return false;

    // BIND RENDER TARGET + DEPTH BUFFER
    d3d11DevCon->OMSetRenderTargets(1, &renderTargetView, depthStencilView);

    // set a viewport
    D3D11_VIEWPORT viewport = {};
    viewport.Width = Width;
    viewport.Height = Height;
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.MinDepth = 0.0f; // default 0.0f
    viewport.MaxDepth = 1.0f;
    d3d11DevCon->RSSetViewports(1, &viewport);

    // camera initializes at point (0, 3, -8) looking at (0, 1, 0) with y-axis as up
    camPosition = XMVectorSet(0.0f, 3.0f, -8.0f, 0.0f);
    camTarget = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    camUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

    // create camera matrices
    camView = XMMatrixLookAtLH(camPosition, camTarget, camUp);

    // create projection matrix for camera
    float fov = XMConvertToRadians(45.0f);
    float aspectRatio = (float)Width / Height;
    camProjection = XMMatrixPerspectiveFovLH(fov, aspectRatio, 0.1f, 100.0f);

    // create world matrix
    World = XMMatrixIdentity();
    WVP = World * camView * camProjection;

    // create constant buffer for camera
    D3D11_BUFFER_DESC cbbd = {};
    cbbd.Usage = D3D11_USAGE_DEFAULT;
    cbbd.ByteWidth = sizeof(cbPerObject);
    cbbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbbd.CPUAccessFlags = 0;
    cbbd.MiscFlags = 0;

    hr = d3d11Device->CreateBuffer(&cbbd, NULL, &cbPerObjectBuffer);
    if (FAILED(hr))
    {
        MessageBox(0, L"Failed to create constant buffer", L"Error", MB_OK);
        return false;
    }

    if (!CreateCube())
    {
        MessageBoxW(0, L"Failed to create square", L"Error", MB_OK);
        return false;
    }

    return true;
}

// draw scene
void DrawScene()
{
    // clear backbuffer
    float bgColor[4] = {0.0f, 0.0f, 0.2f, 1.0f};
    d3d11DevCon->ClearRenderTargetView(renderTargetView, bgColor);
    d3d11DevCon->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    UINT stride = sizeof(Vertex);
    UINT offset = 0;

    d3d11DevCon->IASetVertexBuffers(0, 1, &squareVertexBuffer, &stride, &offset);
    d3d11DevCon->IASetIndexBuffer(squareIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
    d3d11DevCon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // draw cube 1
    WVP = cube1World * camView * camProjection;
    cbPerObj.WVP = XMMatrixTranspose(WVP);
    d3d11DevCon->UpdateSubresource(cbPerObjectBuffer, 0, NULL, &cbPerObj, 0, 0);
    d3d11DevCon->VSSetConstantBuffers(0, 1, &cbPerObjectBuffer);
    d3d11DevCon->DrawIndexed(36, 0, 0);

    // draw cube 2
    WVP = cube2World * camView * camProjection;
    cbPerObj.WVP = XMMatrixTranspose(WVP);
    d3d11DevCon->UpdateSubresource(cbPerObjectBuffer, 0, NULL, &cbPerObj, 0, 0);
    d3d11DevCon->VSSetConstantBuffers(0, 1, &cbPerObjectBuffer);
    d3d11DevCon->DrawIndexed(36, 0, 0);

    swapChain->Present(0, 0);
}

void InitializeCamera()
{
    // static cam set to position (0, 3, -8), looking towards the origin (0, 0, 0)
    camPosition = XMVectorSet(0.0f, 3.0f, -8.0f, 0.0f);
    camTarget = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
    camUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

    // create static view matrix
    camView = XMMatrixLookAtLH(camPosition, camTarget, camUp);
}

void UpdateScene()
{
    // rotate cubes
    static float rot = 0.0f;
    rot += 0.001f;

    // reset world matrices
    cube1World = XMMatrixIdentity();
    cube2World = XMMatrixIdentity();

    // cube 1: rotate and translate
    XMMATRIX Rotation1 = XMMatrixRotationY(rot);
    XMMATRIX Translation1 = XMMatrixTranslation(-2.0f, 0.0f, 4.0f);
    cube1World = Rotation1 * Translation1; // Apply rotation THEN translation

    // cube 2: rotate in opposite direction, scale, and translate
    XMMATRIX Rotation2 = XMMatrixRotationY(-rot);
    XMMATRIX Scale2 = XMMatrixScaling(1.3f, 1.3f, 1.3f);
    XMMATRIX Translation2 = XMMatrixTranslation(2.0f, 1.0f, 6.0f);
    cube2World = Scale2 * Rotation2 * Translation2; // scale first, then rotate, then translate
}

// some fuckass procedure to launch win32 window, thank god for tutorials prayge
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_KEYDOWN:
        if (wParam == VK_ESCAPE)
            DestroyWindow(hwnd);
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

// release objects
// clear this shit up, will probably use comPtr for better memory management
void ReleaseObjects()
{
    if (swapChain)
        swapChain->Release();
    if (d3d11Device)
        d3d11Device->Release();
    if (d3d11DevCon)
        d3d11DevCon->Release();
    if (renderTargetView)
        renderTargetView->Release();
    if (squareVertexBuffer)
        squareVertexBuffer->Release();
    if (squareIndexBuffer)
        squareIndexBuffer->Release();

    // CLEANUP DEPTH BUFFER
    if (depthStencilView)
        depthStencilView->Release();
    if (depthStencilBuffer)
        depthStencilBuffer->Release();
}