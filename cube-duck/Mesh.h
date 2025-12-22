#pragma once

#include <vector>
#include "Core.h"
#include "Math.h"

struct STATIC_VERTEX {
    Vec3 pos;
    Vec3 normal;
    Vec3 tangent;
    float tu;
    float tv;
};

struct PRIM_VERTEX {
    Vec3 position;
    Colour colour;
};

struct VertexDefaultShaderCB {
    Matrix W;
    Matrix VP;
};

STATIC_VERTEX addVertex(Vec3 p, Vec3 n, float tu, float tv, Vec3 tangent = Vec3(0,0,0)) {
    STATIC_VERTEX v;
    v.pos = p;
    v.normal = n;
    v.tangent = tangent;
    v.tangent = Vec3(0, 0, 0); // For now
    v.tu = tu;
    v.tv = tv;
    return v;
}

class VertexLayoutCache {
public:
    static const D3D12_INPUT_LAYOUT_DESC& getStaticLayout() {
        static const D3D12_INPUT_ELEMENT_DESC inputLayoutStatic[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT,
        D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT,
        D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT,
        D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT,
        D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "WORLD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
        { "WORLD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
        { "WORLD", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
        { "WORLD", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
        };
        static const D3D12_INPUT_LAYOUT_DESC desc = { inputLayoutStatic, 8 };
        return desc;
    }
};

class Mesh {
public:
    ID3D12Resource* vertexBuffer;
    ID3D12Resource* indexBuffer;
    ID3D12Resource* instancingBuffer;
    D3D12_VERTEX_BUFFER_VIEW vbView;
    D3D12_INDEX_BUFFER_VIEW ibView;
    D3D12_VERTEX_BUFFER_VIEW instBufferView;
    D3D12_INPUT_LAYOUT_DESC inputLayoutDesc;
    unsigned int numMeshIndices;
    int numInstances;

    Mesh() {}

    void init(Core* core, void* vertices, int vertexSizeInBytes, int numVertices, unsigned int* indices, int numIndices, void* worldInstances, int _numInstances) {
        numInstances = _numInstances;
        
        D3D12_HEAP_PROPERTIES heapprops = {};
        heapprops.Type = D3D12_HEAP_TYPE_DEFAULT;
        heapprops.CreationNodeMask = 1;
        heapprops.VisibleNodeMask = 1;

        D3D12_RESOURCE_DESC vbDesc = {}; // vertex buffer descriptor
        vbDesc.Width = numVertices * vertexSizeInBytes;
        vbDesc.Height = 1;
        vbDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
        vbDesc.DepthOrArraySize = 1;
        vbDesc.MipLevels = 1;
        vbDesc.SampleDesc.Count = 1;
        vbDesc.SampleDesc.Quality = 0;
        vbDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
        core->device->CreateCommittedResource(&heapprops, D3D12_HEAP_FLAG_NONE, &vbDesc,
            D3D12_RESOURCE_STATE_COMMON, NULL, IID_PPV_ARGS(&vertexBuffer));

        core->uploadResource(vertexBuffer, vertices, numVertices * vertexSizeInBytes, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);

        vbView.BufferLocation = vertexBuffer->GetGPUVirtualAddress(); // point where the buffer is located in GPU memory
        vbView.StrideInBytes = vertexSizeInBytes; // size of each vertex. Everytime we step forward in the buffer, we move by this size
        vbView.SizeInBytes = numVertices * vertexSizeInBytes;

        D3D12_RESOURCE_DESC ibDesc; // index buffer descriptor
        memset(&ibDesc, 0, sizeof(D3D12_RESOURCE_DESC));
        ibDesc.Width = numIndices * sizeof(unsigned int);
        ibDesc.Height = 1;
        ibDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
        ibDesc.DepthOrArraySize = 1;
        ibDesc.MipLevels = 1;
        ibDesc.SampleDesc.Count = 1;
        ibDesc.SampleDesc.Quality = 0;
        ibDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
        HRESULT hr;
        hr = core->device->CreateCommittedResource(&heapprops, D3D12_HEAP_FLAG_NONE, &ibDesc,
            D3D12_RESOURCE_STATE_COMMON, NULL, IID_PPV_ARGS(&indexBuffer));
        if (FAILED(hr)) {
            MessageBoxA(NULL, "Failed to create index buffer", "Index Buffer Error", MB_OK | MB_ICONERROR);
            return;
        }
        core->uploadResource(indexBuffer, indices, numIndices * sizeof(unsigned int),
            D3D12_RESOURCE_STATE_INDEX_BUFFER);

        ibView.BufferLocation = indexBuffer->GetGPUVirtualAddress();
        ibView.Format = DXGI_FORMAT_R32_UINT;
        ibView.SizeInBytes = numIndices * sizeof(unsigned int);
        numMeshIndices = numIndices;

        D3D12_RESOURCE_DESC instDesc = {}; // instancing descriptor
        instDesc.Width = numInstances * (16 * sizeof(float)); // World Matrix size
        instDesc.Height = 1;
        instDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
        instDesc.DepthOrArraySize = 1;
        instDesc.MipLevels = 1;
        instDesc.SampleDesc.Count = 1;
        instDesc.SampleDesc.Quality = 0;
        instDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
        hr = core->device->CreateCommittedResource(&heapprops, D3D12_HEAP_FLAG_NONE, &instDesc,
            D3D12_RESOURCE_STATE_COMMON, NULL, IID_PPV_ARGS(&instancingBuffer));
        if (FAILED(hr)) {
            MessageBoxA(NULL, "Failed to create instancing buffer", "Instancing Buffer Error", MB_OK | MB_ICONERROR);
            return;
        }
        core->uploadResource(instancingBuffer, worldInstances, numInstances * (16 * sizeof(float)), D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
        instBufferView.BufferLocation = instancingBuffer->GetGPUVirtualAddress(); // point where the buffer is located in GPU memory
        instBufferView.StrideInBytes = (16 * sizeof(float)); // size of each
        instBufferView.SizeInBytes = numInstances * (16 * sizeof(float));
    }

    void initFromVec(Core* core, std::vector<STATIC_VERTEX> vertices, std::vector<unsigned int> indices, std::vector<Matrix> worldMatrices) {
        init(core, &vertices[0], sizeof(STATIC_VERTEX), vertices.size(), &indices[0], indices.size(), &worldMatrices[0], worldMatrices.size());
        inputLayoutDesc = VertexLayoutCache::getStaticLayout();
    }

    void draw(Core* core) {
        D3D12_VERTEX_BUFFER_VIEW bufferViews[2];
        bufferViews[0] = vbView;
        bufferViews[1] = instBufferView;

        core->getCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        core->getCommandList()->IASetVertexBuffers(0, 2, bufferViews);
        core->getCommandList()->IASetIndexBuffer(&ibView);
        core->getCommandList()->DrawIndexedInstanced(numMeshIndices, numInstances, 0, 0, 0);
    }

};