#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>
#include <wrl/client.h>
#include <string>
#include <unordered_map>
#include <memory>
#include "ResourceManager.h"

class ShaderManager
{
public:
    ShaderManager(ID3D11Device *device);
    ShaderManager(std::shared_ptr<ResourceManager> resourceManager);
    ~ShaderManager() = default;

    bool CompileAndCreateVertexShader(const std::wstring &filename,
                                      const std::string &entryPoint,
                                      Microsoft::WRL::ComPtr<ID3D11VertexShader> &shader,
                                      Microsoft::WRL::ComPtr<ID3DBlob> &shaderBlob);

    bool CompileAndCreatePixelShader(const std::wstring &filename,
                                     const std::string &entryPoint,
                                     Microsoft::WRL::ComPtr<ID3D11PixelShader> &shader);

    bool CreateInputLayout(const D3D11_INPUT_ELEMENT_DESC *layout,
                           UINT numElements,
                           ID3DBlob *shaderBlob,
                           Microsoft::WRL::ComPtr<ID3D11InputLayout> &inputLayout);

private:
    Microsoft::WRL::ComPtr<ID3D11Device> device;
    std::shared_ptr<ResourceManager> resourceManager = nullptr;

    bool CompileShaderFromFile(const std::wstring &filename,
                               const std::string &entryPoint,
                               const std::string &shaderModel,
                               Microsoft::WRL::ComPtr<ID3DBlob> &shaderBlob);
};