// define UNICODE to ensure the application uses wide-character strings
// not sure why it's like this but as long as it compiles my code, i'm happy
#define UNICODE

// other necessary headers
#include <windows.h>
#include <windowsx.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include "WICTextureLoader.h"
#include <string>

// link libraries
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

// not sure if using namespace is a good idea but i can always change it later
using namespace DirectX;

// main class
class DirectXRenderer
{
public:
    DirectXRenderer(HINSTANCE hInstance);
    ~DirectXRenderer();
    bool Initialize();
    void Run();

private:
    bool InitWindow();
    bool InitDirectX();
    void Update();
    void Render();
    void Cleanup();

    // camera members
    XMFLOAT3 cameraPosition;
    float cameraYaw;
    float cameraPitch;
    bool leftMouseDown;
    POINT prevMousePos;

    // directx members
    HINSTANCE hInstance;
    HWND hWnd;
    D3D_DRIVER_TYPE driverType;
    D3D_FEATURE_LEVEL featureLevel;

    // device and swap chain
    ID3D11Device *device = nullptr;
    ID3D11DeviceContext *context = nullptr;
    IDXGISwapChain *swapChain = nullptr;

    // render target view
    ID3D11RenderTargetView *renderTargetView = nullptr;

    // shaders and input layout
    ID3D11VertexShader *vertexShader = nullptr;
    ID3D11PixelShader *pixelShader = nullptr;
    ID3D11InputLayout *inputLayout = nullptr;

    // buffers
    ID3D11Buffer *vertexBuffer = nullptr;
    ID3D11Buffer *indexBuffer = nullptr;
    ID3D11Buffer *constantBuffer = nullptr;

    // depth/stencil buffer
    ID3D11DepthStencilView *depthStencilView = nullptr;

    // texture
    ID3D11SamplerState *samplerState = nullptr;
    ID3D11ShaderResourceView *textureRV = nullptr;

    // create a wireframe rendering mode
    ID3D11RasterizerState *solidRasterizerState = nullptr;
    ID3D11RasterizerState *wireframeRasterizerState = nullptr;
    bool wireframeMode = false; // toggle wireframe mode

    float rotationAngle;

    static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};

// vertex structure
struct Vertex
{
    XMFLOAT3 position;
    XMFLOAT4 color;
    XMFLOAT2 texCoord;
};

// constant buffer structure
struct ConstantBuffer
{
    XMMATRIX wvp; // World-View-Projection matrix
};

// constructor and destructor
DirectXRenderer::DirectXRenderer(HINSTANCE hInstance)
    : hInstance(hInstance),
      hWnd(nullptr),
      driverType(D3D_DRIVER_TYPE_HARDWARE),
      featureLevel(D3D_FEATURE_LEVEL_11_0),
      device(nullptr),
      context(nullptr),
      swapChain(nullptr),
      renderTargetView(nullptr),
      vertexShader(nullptr),
      pixelShader(nullptr),
      inputLayout(nullptr),
      vertexBuffer(nullptr),
      indexBuffer(nullptr),
      constantBuffer(nullptr),
      depthStencilView(nullptr),
      rotationAngle(0.0f),
      cameraPosition(0.0f, 0.0f, -3.0f),
      cameraYaw(0.0f),
      cameraPitch(0.0f),
      leftMouseDown(false)
{
    prevMousePos.x = 0;
    prevMousePos.y = 0;
}

DirectXRenderer::~DirectXRenderer()
{
    Cleanup();
}

// initialize window and directx
bool DirectXRenderer::Initialize()
{
    if (!InitWindow())
        return false;
    if (!InitDirectX())
        return false;
    return true;
}

// essentially the main loop
void DirectXRenderer::Run()
{
    MSG msg = {0};
    while (WM_QUIT != msg.message)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            Update();
            Render();
        }
    }
}

// initialize window and directx
bool DirectXRenderer::InitWindow()
{
    WNDCLASSEX wc = {0};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.lpszClassName = L"DirectXWindowClass";

    if (!RegisterClassEx(&wc))
        return false;

    RECT rc = {0, 0, 800, 600};
    AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
    hWnd = CreateWindow(
        L"DirectXWindowClass",
        L"DirectX Renderer",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        rc.right - rc.left, rc.bottom - rc.top,
        nullptr,
        nullptr,
        hInstance,
        this // pass 'this' pointer to the window procedure
    );

    // if window creation failed
    if (!hWnd)
        return false;

    ShowWindow(hWnd, SW_SHOW);

    return true;
}

// initialize directx
bool DirectXRenderer::InitDirectX()
{
    DXGI_SWAP_CHAIN_DESC sd = {0};
    sd.BufferCount = 1;
    sd.BufferDesc.Width = 800;
    sd.BufferDesc.Height = 600;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // standard 32-bit colour format
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;

    HRESULT hr = D3D11CreateDeviceAndSwapChain(nullptr, driverType, nullptr, 0, nullptr, 0,
                                               D3D11_SDK_VERSION, &sd, &swapChain, &device, &featureLevel, &context);
    if (FAILED(hr))
        return false;

    D3D11_RASTERIZER_DESC rasterDesc;
    ZeroMemory(&rasterDesc, sizeof(rasterDesc));
    rasterDesc.CullMode = D3D11_CULL_BACK;
    rasterDesc.FrontCounterClockwise = false;
    rasterDesc.DepthClipEnable = true;

    // create solid rasterizer state
    rasterDesc.FillMode = D3D11_FILL_SOLID;
    hr = device->CreateRasterizerState(&rasterDesc, &solidRasterizerState);
    if (FAILED(hr))
        return false;

    // create wireframe rasterizer state
    rasterDesc.FillMode = D3D11_FILL_WIREFRAME;
    hr = device->CreateRasterizerState(&rasterDesc, &wireframeRasterizerState);
    if (FAILED(hr))
        return false;

    ID3D11Texture2D *pBackBuffer = nullptr;
    hr = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID *)&pBackBuffer);
    if (FAILED(hr))
        return false;

    hr = device->CreateRenderTargetView(pBackBuffer, nullptr, &renderTargetView);
    pBackBuffer->Release();
    if (FAILED(hr))
        return false;

    // describe depth/stencil buffer
    D3D11_TEXTURE2D_DESC depthStencilDesc = {0};
    depthStencilDesc.Width = 800;
    depthStencilDesc.Height = 600;
    depthStencilDesc.MipLevels = 1;
    depthStencilDesc.ArraySize = 1;
    depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthStencilDesc.SampleDesc.Count = 1;
    depthStencilDesc.SampleDesc.Quality = 0;
    depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
    depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depthStencilDesc.CPUAccessFlags = 0;
    depthStencilDesc.MiscFlags = 0;

    ID3D11Texture2D *depthStencilBuffer = nullptr;
    hr = device->CreateTexture2D(&depthStencilDesc, nullptr, &depthStencilBuffer);
    if (FAILED(hr))
        return false;

    // create depth/stencil view
    hr = device->CreateDepthStencilView(depthStencilBuffer, nullptr, &this->depthStencilView);
    depthStencilBuffer->Release();
    if (FAILED(hr))
        return false;

    // bind render target and depth/stencil view to the output-merger stage
    context->OMSetRenderTargets(1, &renderTargetView, depthStencilView);

    // texture loading
    // no png set yet
    // save png to assets folder and change the path
    hr = CreateWICTextureFromFile(device, context, L"assets/crying_rat.png", nullptr, &textureRV);
    if (FAILED(hr))
        return false;

    D3D11_SAMPLER_DESC sampDesc;
    ZeroMemory(&sampDesc, sizeof(sampDesc));
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

    hr = device->CreateSamplerState(&sampDesc, &samplerState);
    if (FAILED(hr))
        return false;

    // set the viewport
    D3D11_VIEWPORT vp;
    vp.Width = (FLOAT)800;
    vp.Height = (FLOAT)600;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    context->RSSetViewports(1, &vp);

    // compiles vertex shader from hlsl file. if fail, check for:
    // incorrect file path
    // hlsl syntax errors
    // missing shader model (vs_5_0)
    ID3DBlob *vsBlob = nullptr;
    hr = D3DCompileFromFile(L"shaders/vertexShader.hlsl", nullptr, nullptr, "main", "vs_5_0", 0, 0, &vsBlob, nullptr);
    if (FAILED(hr))
        return false;

    hr = device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &vertexShader);
    if (FAILED(hr))
    {
        vsBlob->Release();
        return false;
    }

    // define input layout
    D3D11_INPUT_ELEMENT_DESC layout[] = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 28, D3D11_INPUT_PER_VERTEX_DATA, 0}};
    UINT numElements = ARRAYSIZE(layout);

    hr = device->CreateInputLayout(layout, numElements, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &inputLayout);
    vsBlob->Release();
    if (FAILED(hr))
        return false;

    context->IASetInputLayout(inputLayout);

    // compiles pixel shader from hlsl file. if fail, check for:
    // incorrect file path
    // hlsl syntax errors
    // missing shader model (ps_5_0)
    ID3DBlob *psBlob = nullptr;
    hr = D3DCompileFromFile(L"shaders/pixelShader.hlsl", nullptr, nullptr, "main", "ps_5_0", 0, 0, &psBlob, nullptr);
    if (FAILED(hr))
        return false;

    hr = device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &pixelShader);
    psBlob->Release();
    if (FAILED(hr))
        return false;

    // define vertices (counter-clockwise order)
    Vertex cubeVertices[] =
        {
            //              position                  colour      texture coordinates
            {XMFLOAT3(-0.5f, 0.5f, -0.5f), XMFLOAT4(1, 0, 0, 1), XMFLOAT2(0.0f, 0.0f)},  // 0: front-top-left
            {XMFLOAT3(0.5f, 0.5f, -0.5f), XMFLOAT4(0, 1, 0, 1), XMFLOAT2(1.0f, 0.0f)},   // 1: front-top-right
            {XMFLOAT3(0.5f, -0.5f, -0.5f), XMFLOAT4(0, 0, 1, 1), XMFLOAT2(1.0f, 1.0f)},  // 2: front-bottom-right
            {XMFLOAT3(-0.5f, -0.5f, -0.5f), XMFLOAT4(1, 1, 1, 1), XMFLOAT2(0.0f, 1.0f)}, // 3: front-bottom-left

            {XMFLOAT3(-0.5f, 0.5f, 0.5f), XMFLOAT4(1, 1, 0, 1), XMFLOAT2(1.0f, 0.0f)}, // 4: back-top-left
            {XMFLOAT3(0.5f, 0.5f, 0.5f), XMFLOAT4(0, 1, 1, 1), XMFLOAT2(0.0f, 0.0f)},  // 5: back-top-right
            {XMFLOAT3(0.5f, -0.5f, 0.5f), XMFLOAT4(1, 0, 1, 1), XMFLOAT2(0.0f, 1.0f)}, // 6: back-bottom-right
            {XMFLOAT3(-0.5f, -0.5f, 0.5f), XMFLOAT4(0, 0, 0, 1), XMFLOAT2(1.0f, 1.0f)} // 7: back-bottom-left
        };

    // define indices
    UINT cubeIndices[] =
        {
            // front face
            0, 1, 2,
            0, 2, 3,

            // back face
            5, 4, 7,
            5, 7, 6,

            // left face
            4, 0, 3,
            4, 3, 7,

            // right face
            1, 5, 6,
            1, 6, 2,

            // top face
            4, 5, 1,
            4, 1, 0,

            // bottom face
            3, 2, 6,
            3, 6, 7};

    // create vertex buffer
    D3D11_BUFFER_DESC bd = {0};
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(cubeVertices);
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA initData = {0};
    initData.pSysMem = cubeVertices;

    hr = device->CreateBuffer(&bd, &initData, &vertexBuffer);
    if (FAILED(hr))
        return false;

    // create index buffer
    D3D11_BUFFER_DESC ibd = {0};
    ibd.Usage = D3D11_USAGE_DEFAULT;
    ibd.ByteWidth = sizeof(cubeIndices);
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;

    D3D11_SUBRESOURCE_DATA iinitData = {};
    iinitData.pSysMem = cubeIndices;

    hr = device->CreateBuffer(&ibd, &iinitData, &indexBuffer);
    if (FAILED(hr))
        return false;

    // create constant buffer
    D3D11_BUFFER_DESC cbd = {};
    cbd.Usage = D3D11_USAGE_DEFAULT;
    cbd.ByteWidth = sizeof(ConstantBuffer);
    cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbd.CPUAccessFlags = 0;

    hr = device->CreateBuffer(&cbd, nullptr, &constantBuffer);
    if (FAILED(hr))
        return false;

    return true;
}

// update
void DirectXRenderer::Update()
{
    // transformations
    // cube rotation
    rotationAngle += 0.0005f; // arbitrary value, can be adjusted

    // other logic can be added here

    // camera movement
    float moveSpeed = 0.002f;
    float cosPitch = cosf(cameraPitch);
    float sinPitch = sinf(cameraPitch);
    float cosYaw = cosf(cameraYaw);
    float sinYaw = sinf(cameraYaw);

    // define move forward
    XMVECTOR forward = XMVectorSet(
        cosPitch * sinYaw,
        sinPitch,
        cosPitch * cosYaw,
        0.0f);
    forward = XMVector3Normalize(forward);

    // define move right by defining the cross product of forward and up
    XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    XMVECTOR right = XMVector3Cross(up, forward);
    right = XMVector3Normalize(right);

    // handle input
    if (GetAsyncKeyState('W') & 0x8000)
    {
        XMVECTOR pos = XMLoadFloat3(&cameraPosition);
        pos = XMVectorAdd(pos, XMVectorScale(forward, moveSpeed));
        XMStoreFloat3(&cameraPosition, pos);
    }
    if (GetAsyncKeyState('S') & 0x8000)
    {
        XMVECTOR pos = XMLoadFloat3(&cameraPosition);
        pos = XMVectorSubtract(pos, XMVectorScale(forward, moveSpeed));
        XMStoreFloat3(&cameraPosition, pos);
    }
    if (GetAsyncKeyState('A') & 0x8000)
    {
        XMVECTOR pos = XMLoadFloat3(&cameraPosition);
        pos = XMVectorSubtract(pos, XMVectorScale(right, moveSpeed));
        XMStoreFloat3(&cameraPosition, pos);
    }
    if (GetAsyncKeyState('D') & 0x8000)
    {
        XMVECTOR pos = XMLoadFloat3(&cameraPosition);
        pos = XMVectorAdd(pos, XMVectorScale(right, moveSpeed));
        XMStoreFloat3(&cameraPosition, pos);
    }
}

// render
void DirectXRenderer::Render()
{
    // set rasterizer state
    if (wireframeMode)
    {
        context->RSSetState(wireframeRasterizerState);
    }
    else
    {
        context->RSSetState(solidRasterizerState);
    }

    // clear colour buffer
    float clearColor[4] = {0.0f, 0.2f, 0.4f, 1.0f}; // (RGBA) some blueish colour
    context->ClearRenderTargetView(renderTargetView, clearColor);

    // clear depth/stencil buffer
    context->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    // build view matrix from camera variables
    XMVECTOR eye = XMLoadFloat3(&cameraPosition);
    float cosPitch = cosf(cameraPitch);
    float sinPitch = sinf(cameraPitch);
    float cosYaw = cosf(cameraYaw);
    float sinYaw = sinf(cameraYaw);

    XMVECTOR forward = XMVectorSet(
        cosPitch * sinYaw,
        sinPitch,
        cosPitch * cosYaw,
        0.0f);

    forward = XMVector3Normalize(forward);

    XMVECTOR target = XMVectorAdd(eye, forward);
    XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    XMMATRIX view = XMMatrixLookAtLH(eye, target, up);
    XMMATRIX projection = XMMatrixPerspectiveFovLH(XM_PIDIV2, 800.0f / 600.0f, 0.01f, 100.0f);

    // bind texture resource and sampler state
    context->PSSetShaderResources(0, 1, &textureRV);
    context->PSSetSamplers(0, 1, &samplerState);

    // setup pipeline
    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
    context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    context->VSSetShader(vertexShader, nullptr, 0);
    context->PSSetShader(pixelShader, nullptr, 0);

    // combine into the wvp matrix and transpose it for the shaders
    ConstantBuffer cb;

    // create animations for two cubes
    // cube 1 - rotates around the y-axis
    XMMATRIX world1 = XMMatrixRotationY(rotationAngle) * XMMatrixTranslation(-1.5f, 0.0f, 0.0f);
    cb.wvp = XMMatrixTranspose(world1 * view * projection);
    context->UpdateSubresource(constantBuffer, 0, nullptr, &cb, 0, 0);
    context->VSSetConstantBuffers(0, 1, &constantBuffer);
    context->DrawIndexed(36, 0, 0);

    // cube 2 - rotates around the x-axis
    XMMATRIX world2 = XMMatrixRotationX(rotationAngle) * XMMatrixTranslation(1.5f, 0.0f, 0.0f);
    cb.wvp = XMMatrixTranspose(world2 * view * projection);
    context->UpdateSubresource(constantBuffer, 0, nullptr, &cb, 0, 0);
    context->VSSetConstantBuffers(0, 1, &constantBuffer);
    context->DrawIndexed(36, 0, 0);

    swapChain->Present(0, 0);
}

// cleanup resources before closing since im using raw pointers
// i know i should be using smart pointers but i couldn't get them to work for the life of me
// problem for future me
void DirectXRenderer::Cleanup()
{
    // clear gpu state before releasing resources
    if (context)
        context->ClearState();

    // release resources
    if (depthStencilView)
        depthStencilView->Release();
    if (vertexBuffer)
        vertexBuffer->Release();
    if (inputLayout)
        inputLayout->Release();
    if (vertexShader)
        vertexShader->Release();
    if (pixelShader)
        pixelShader->Release();
    if (renderTargetView)
        renderTargetView->Release();
    if (swapChain)
        swapChain->Release();
    if (constantBuffer)
        constantBuffer->Release();
    if (indexBuffer)
        indexBuffer->Release();
    if (solidRasterizerState)
        solidRasterizerState->Release();
    if (wireframeRasterizerState)
        wireframeRasterizerState->Release();
    if (samplerState)
        samplerState->Release();

    if (context)
        context->Release();
    if (device)
        device->Release();
}

// window procedure to handle OS messages
// processes events
// right now, it's handling a quit message
// handles rasterizer state
// future changes include handling camera movement and input
LRESULT CALLBACK DirectXRenderer::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    DirectXRenderer *pThis = reinterpret_cast<DirectXRenderer *>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

    switch (message)
    {
    case WM_CREATE:
    {
        // store the 'this' pointer in the window user data since wireframemode = !wireframemode; can't access it directly
        LPCREATESTRUCT pCreateStruct = reinterpret_cast<LPCREATESTRUCT>(lParam);
        DirectXRenderer *pThis = reinterpret_cast<DirectXRenderer *>(pCreateStruct->lpCreateParams);

        SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis));
        return 0;
    }

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    case WM_KEYDOWN:
    {
        DirectXRenderer *pThis = reinterpret_cast<DirectXRenderer *>(
            GetWindowLongPtr(hWnd, GWLP_USERDATA));

        if (pThis)
        {
            // r for rasterizer
            // toggles wireframe mode
            if (wParam == 'R')
            {
                pThis->wireframeMode = !pThis->wireframeMode;
            }
        }
        break;
    }

    case WM_LBUTTONDOWN:
    {
        if (pThis)
        {
            pThis->leftMouseDown = true;
            pThis->prevMousePos.x = GET_X_LPARAM(lParam);
            pThis->prevMousePos.y = GET_Y_LPARAM(lParam);
        }
        break;
    }

    case WM_LBUTTONUP:
    {
        if (pThis)
        {
            pThis->leftMouseDown = false;
        }
        break;
    }

    case WM_MOUSEMOVE:
    {
        if (pThis && pThis->leftMouseDown)
        {
            int mouseX = GET_X_LPARAM(lParam);
            int mouseY = GET_Y_LPARAM(lParam);
            int deltaX = mouseX - pThis->prevMousePos.x;
            int deltaY = mouseY - pThis->prevMousePos.y;

            float sensitivity = 0.002f;
            pThis->cameraYaw += deltaX * sensitivity;
            pThis->cameraPitch -= deltaY * sensitivity;

            // clamp pitch to prevent flipping
            if (pThis->cameraPitch > XM_PIDIV2 - 0.1f)
                pThis->cameraPitch = XM_PIDIV2 - 0.1f;
            if (pThis->cameraPitch < -XM_PIDIV2 + 0.1f)
                pThis->cameraPitch = -XM_PIDIV2 + 0.1f;

            pThis->prevMousePos.x = mouseX;
            pThis->prevMousePos.y = mouseY;
        }
        break;
    }

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// entry point
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
    DirectXRenderer renderer(hInstance);

    if (!renderer.Initialize())
    {
        MessageBox(nullptr, L"Failed to initialize DirectXRenderer.", L"Error", MB_OK | MB_ICONERROR);
        return -1;
    }

    renderer.Run();

    return 0;
}
