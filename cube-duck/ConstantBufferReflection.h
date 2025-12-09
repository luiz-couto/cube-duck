#pragma once

#include <string>
#include <map>
#include "Core.h"
#include "ShaderManager.h"

struct ConstantBufferVariable {
    unsigned int offset;
    unsigned int size;
};

class ConstantBufferReflection {
public:
    std::string name;
    std::map<std::string, ConstantBufferVariable> constantBufferData;

    ID3D12Resource* constantBuffer;
    unsigned char* buffer;
    unsigned int cbSizeInBytes;
    unsigned int maxDrawCalls;
    unsigned int offsetIndex;

    void init(Core* core, unsigned int sizeInBytes, unsigned int _maxDrawCalls = 1024) {
        maxDrawCalls = _maxDrawCalls;
        cbSizeInBytes = (sizeInBytes + 255) & ~255;
        unsigned int cbSizeInBytesAligned = cbSizeInBytes * maxDrawCalls;
        offsetIndex = 0;
        HRESULT hr;
        D3D12_HEAP_PROPERTIES heapprops = {};
        heapprops.Type = D3D12_HEAP_TYPE_UPLOAD;
        heapprops.CreationNodeMask = 1;
        heapprops.VisibleNodeMask = 1;
        D3D12_RESOURCE_DESC cbDesc = {};
        cbDesc.Width = cbSizeInBytesAligned;
        cbDesc.Height = 1;
        cbDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
        cbDesc.DepthOrArraySize = 1;
        cbDesc.MipLevels = 1;
        cbDesc.SampleDesc.Count = 1;
        cbDesc.SampleDesc.Quality = 0;
        cbDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
        hr = core->device->CreateCommittedResource(&heapprops, D3D12_HEAP_FLAG_NONE, &cbDesc, D3D12_RESOURCE_STATE_GENERIC_READ, NULL,
            IID_PPV_ARGS(&constantBuffer));
        if (FAILED(hr)) {
            MessageBoxA(NULL, "Failed to create constant buffer", "Constant Buffer Error", MB_OK | MB_ICONERROR);
            return;
        }
        constantBuffer->Map(0, NULL, (void**)&buffer);
    }

    D3D12_GPU_VIRTUAL_ADDRESS getGPUAddress() const {
        return (constantBuffer->GetGPUVirtualAddress() + (offsetIndex * cbSizeInBytes));
    }

    void update(std::string name, void* data) {
        if (constantBufferData.find(name) == constantBufferData.end()) {
            MessageBoxA(NULL, ("Constant buffer variable not found: " + name).c_str(), "Error", MB_OK | MB_ICONERROR);
            return;
        }
        ConstantBufferVariable cbVariable = constantBufferData[name];
        unsigned int offset = offsetIndex * cbSizeInBytes;
        memcpy(&buffer[offset + cbVariable.offset], data, cbVariable.size);
    }

    void next() {
        offsetIndex++;
        if (offsetIndex >= maxDrawCalls) {
            offsetIndex = 0;
        }
    }
};