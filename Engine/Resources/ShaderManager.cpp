#include "ShaderManager.h"
#include <vector>
#include <string>
#include <stdexcept>

ShaderManager::ShaderManager(ID3D11Device *device)
    : device(device), resourceManager(nullptr) {}

ShaderManager::ShaderManager(std::shared_ptr<ResourceManager> resourceManager)
    : resourceManager(resourceManager), device(resourceManager ? resourceManager->GetDevice() : nullptr) {}

// default destructor

bool ShaderManager::CompileAndCreateVertexShader(
    const std::wstring &filename,
    const std::string &entryPoint,
    Microsoft::WRL::ComPtr<ID3D11VertexShader> &shader,
    Microsoft::WRL::ComPtr<ID3DBlob> &shaderBlob)
{
    if (!CompileShaderFromFile(filename, entryPoint, "vs_5_0", shaderBlob))
        return false;

    // ensure device is valid before use
    if (!device)
    {
        throw std::runtime_error("ShaderManager: device is null. Cannot create vertex shader.");
    }

    HRESULT hr = device->CreateVertexShader(
        shaderBlob->GetBufferPointer(),
        shaderBlob->GetBufferSize(),
        nullptr,
        shader.GetAddressOf());

    return SUCCEEDED(hr);
}

bool ShaderManager::CompileAndCreatePixelShader(
    const std::wstring &filename,
    const std::string &entryPoint,
    Microsoft::WRL::ComPtr<ID3D11PixelShader> &shader)
{
    Microsoft::WRL::ComPtr<ID3DBlob> shaderBlob;
    if (!CompileShaderFromFile(filename, entryPoint, "ps_5_0", shaderBlob))
        return false;

    // ensure device is valid before use
    if (!device)
    {
        throw std::runtime_error("ShaderManager: device is null. Cannot create pixel shader.");
    }

    HRESULT hr = device->CreatePixelShader(
        shaderBlob->GetBufferPointer(),
        shaderBlob->GetBufferSize(),
        nullptr,
        shader.GetAddressOf());

    return SUCCEEDED(hr);
}

bool ShaderManager::CreateInputLayout(
    const D3D11_INPUT_ELEMENT_DESC *layout,
    UINT numElements,
    ID3DBlob *shaderBlob,
    Microsoft::WRL::ComPtr<ID3D11InputLayout> &inputLayout)
{

    // ensure device is valid before use
    if (!device)
    {
        throw std::runtime_error("ShaderManager: device is null. Cannot create input layout.");
    }

    HRESULT hr = device->CreateInputLayout(
        layout,
        numElements,
        shaderBlob->GetBufferPointer(),
        shaderBlob->GetBufferSize(),
        inputLayout.GetAddressOf());

    return SUCCEEDED(hr);
}

bool ShaderManager::CompileShaderFromFile(
    const std::wstring &filename,
    const std::string &entryPoint,
    const std::string &shaderModel,
    Microsoft::WRL::ComPtr<ID3DBlob> &shaderBlob)
{
    // set debug flags in debug mode (program is being compiled in debug mode)
    // i need to add more error handling around the entire program tbh
    UINT compileFlags = 0;
#ifdef _DEBUG
    compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

    Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;
    HRESULT hr = D3DCompileFromFile(
        filename.c_str(),
        nullptr,
        nullptr,
        entryPoint.c_str(),
        shaderModel.c_str(),
        compileFlags,
        0,
        shaderBlob.GetAddressOf(),
        errorBlob.GetAddressOf());

    if (FAILED(hr))
    {
        if (errorBlob)
        {
            std::string errorMessage = static_cast<const char *>(errorBlob->GetBufferPointer());
            OutputDebugStringA(errorMessage.c_str());
        }
        throw std::runtime_error("Failed to compile shader: " + std::string(entryPoint));
    }

    return true;
}