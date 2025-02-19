// for some reason, if i don't include this header file, the compiler will throw a bunch of string errors so pls just keep this here
#define UNICODE

// other necessary headers
#include <windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
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
    void Render();
    void Cleanup();

    HINSTANCE hInstance;
    HWND hWnd;
    D3D_DRIVER_TYPE driverType;
    D3D_FEATURE_LEVEL featureLevel;
    ID3D11Device *device;
    ID3D11DeviceContext *context;
    IDXGISwapChain *swapChain;
    ID3D11RenderTargetView *renderTargetView;
    ID3D11VertexShader *vertexShader;
    ID3D11PixelShader *pixelShader;
    ID3D11InputLayout *inputLayout;
    ID3D11Buffer *vertexBuffer;

    static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};

// vertex structure
struct Vertex
{
    XMFLOAT3 position;
    XMFLOAT4 color;
};

// constructor and destructor
DirectXRenderer::DirectXRenderer(HINSTANCE hInstance) : hInstance(hInstance), hWnd(nullptr), driverType(D3D_DRIVER_TYPE_HARDWARE),
                                                        featureLevel(D3D_FEATURE_LEVEL_11_0), device(nullptr), context(nullptr),
                                                        swapChain(nullptr), renderTargetView(nullptr), vertexShader(nullptr),
                                                        pixelShader(nullptr), inputLayout(nullptr), vertexBuffer(nullptr) {}

DirectXRenderer::~DirectXRenderer()
{
    Cleanup();
}

// initialize and run
bool DirectXRenderer::Initialize()
{
    if (!InitWindow())
        return false;
    if (!InitDirectX())
        return false;
    return true;
}

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
    hWnd = CreateWindow(L"DirectXWindowClass", L"DirectX Renderer", WS_OVERLAPPEDWINDOW,
                        CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInstance, nullptr);

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

    ID3D11Texture2D *pBackBuffer = nullptr;
    hr = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID *)&pBackBuffer);
    if (FAILED(hr))
        return false;

    hr = device->CreateRenderTargetView(pBackBuffer, nullptr, &renderTargetView);
    pBackBuffer->Release();
    if (FAILED(hr))
        return false;

    context->OMSetRenderTargets(1, &renderTargetView, nullptr);

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
    };
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

    // create vertex buffer
    Vertex vertices[] = {
        {XMFLOAT3(0.0f, 0.5f, 0.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f)},
        {XMFLOAT3(0.5f, -0.5f, 0.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f)},
        {XMFLOAT3(-0.5f, -0.5f, 0.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f)},
    };

    D3D11_BUFFER_DESC bd = {0};
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(vertices);
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA initData = {0};
    initData.pSysMem = vertices;

    hr = device->CreateBuffer(&bd, &initData, &vertexBuffer);
    if (FAILED(hr))
        return false;

    return true;
}

// render
void DirectXRenderer::Render()
{
    float clearColor[4] = {0.0f, 0.2f, 0.4f, 1.0f}; // (RGBA) some blueish colour
    context->ClearRenderTargetView(renderTargetView, clearColor);

    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    context->VSSetShader(vertexShader, nullptr, 0);
    context->PSSetShader(pixelShader, nullptr, 0);

    context->Draw(3, 0);

    swapChain->Present(0, 0);
}

// cleanup resources before closing since im using raw pointers
// i know i should be using smart pointers but i couldn't get them to work for the life of me
// problem for future me
void DirectXRenderer::Cleanup()
{

    if (context)
        // clear gpu state before releasing resources
        context->ClearState();
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
    if (context)
        context->Release();
    if (device)
        device->Release();
}

// some bullshit window procedure
// window event handler WndProc
// handles OS messages
// if window is destroyed, post quit message
LRESULT CALLBACK DirectXRenderer::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
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
        return -1;

    renderer.Run();

    return 0;
}
