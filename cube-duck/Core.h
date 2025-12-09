#pragma once

#include <d3d12.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>
#include <vector>
#include "Fence.h"
#include "Barrier.h"

#pragma comment(lib, "d3d12")
#pragma comment(lib, "dxgi")
#pragma comment(lib, "d3dcompiler.lib")

class DescriptorHeap {
public:
    ID3D12DescriptorHeap* heap;
    D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle;
    D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle;
    unsigned int incrementSize;
    int used;

    void init(ID3D12Device5* device, int num) {
        D3D12_DESCRIPTOR_HEAP_DESC uavcbvHeapDesc = {};
        uavcbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        uavcbvHeapDesc.NumDescriptors = num;
        uavcbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        device->CreateDescriptorHeap(&uavcbvHeapDesc, IID_PPV_ARGS(&heap));
        cpuHandle = heap->GetCPUDescriptorHandleForHeapStart();
        gpuHandle = heap->GetGPUDescriptorHandleForHeapStart();
        incrementSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
        used = 0;
    }

    D3D12_CPU_DESCRIPTOR_HANDLE getNextCPUHandle() {
        if (used > 0) {
            cpuHandle.ptr += incrementSize;
        }
        used++;
        return cpuHandle;
    }

    // deallocate all descriptors in the future - destructor
    ~DescriptorHeap() {
        heap->Release();
    }

};

class Core {
public:
    int wWidth, wHeight;
    IDXGIAdapter1* adapter;

    ID3D12Device5* device;
    ID3D12CommandQueue* graphicsQueue;
    ID3D12CommandQueue* copyQueue;
    ID3D12CommandQueue* computeQueue;
    IDXGISwapChain3* swapchain;
    ID3D12CommandAllocator* graphicsCommandAllocator[2];
    ID3D12GraphicsCommandList4* graphicsCommandList[2];
    ID3D12DescriptorHeap* backbufferHeap;
    ID3D12Resource** backbuffers;
    ID3D12DescriptorHeap* dsvHeap;
    ID3D12Resource* dsv;
    D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle;
    D3D12_VIEWPORT viewport;
    D3D12_RECT scissorRect;

    ID3D12RootSignature* rootSignature;

    GPUFence graphicsQueueFence[2];

    DescriptorHeap srvHeap;

    Core() {}

    void selectAdapter(IDXGIFactory6* factory) {
        IDXGIAdapter1* adapterf;
        std::vector<IDXGIAdapter1*> adapters;
        int i = 0;

        while (factory->EnumAdapters1(i, &adapterf) != DXGI_ERROR_NOT_FOUND) {
            adapters.push_back(adapterf);
            i++;
        }

        long long maxVideoMemory = 0;
        int useAdapterIndex = 0;
        for (int i = 0; i < adapters.size(); i++) {
            DXGI_ADAPTER_DESC desc;
            adapters[i]->GetDesc(&desc);
            if (desc.DedicatedVideoMemory > maxVideoMemory) {
                maxVideoMemory = desc.DedicatedVideoMemory;
                useAdapterIndex = i;
            }
        }

        adapter = adapters[useAdapterIndex];
        D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(&device));
    }

    void createCommandQueues() {
        D3D12_COMMAND_QUEUE_DESC graphicsQueueDesc = {};
        graphicsQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
        device->CreateCommandQueue(&graphicsQueueDesc, IID_PPV_ARGS(&graphicsQueue));

        D3D12_COMMAND_QUEUE_DESC copyQueueDesc = {};
        copyQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_COPY;
        device->CreateCommandQueue(&copyQueueDesc, IID_PPV_ARGS(&copyQueue));

        D3D12_COMMAND_QUEUE_DESC computeQueueDesc = {};
        computeQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;
        device->CreateCommandQueue(&computeQueueDesc, IID_PPV_ARGS(&computeQueue));
    }

    void initSwapChain(HWND& hwnd, IDXGIFactory6* factory, DXGI_SWAP_CHAIN_DESC1& scDesc) {
        scDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        scDesc.Width = wWidth;
        scDesc.Height = wHeight;
        scDesc.SampleDesc.Count = 1; // MSAA here
        scDesc.SampleDesc.Quality = 0;
        scDesc.BufferCount = 2;
        scDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        IDXGISwapChain1* swapChain1;
        factory->CreateSwapChainForHwnd(graphicsQueue, hwnd, &scDesc, NULL, NULL, &swapChain1);
        swapChain1->QueryInterface(&swapchain);
        swapChain1->Release();
    }

    int frameIndex() {
        return swapchain->GetCurrentBackBufferIndex();
    }

    int swapchainBufferCount() {
        //return swapchain->GetDesc1()->BufferCount;
        return 2;
    }

    // finds the adapter with bigger maxVideoMemory and uses it
    // creates the device
    // creates the 3 command queues: graphics, copy and compute
    // creates the swapchain
    // creates 2 command allocators and command lists
    // creates the backbuffer descriptor heap: 2 descriptors - one for each backbuffer
    // creates the render target views for each backbuffer
    // creates the fences for the graphics queue
    // creates the depth stencil view and resource
    // creates the dsv descriptor heap
    // allocates the depth stencil resource in GPU local memory
    // create viewport and scissor rect
    // creates a basic root signature
    void init(HWND hwnd, int _width, int _height, bool enableDebugLayer = true) {
        wWidth = _width;
        wHeight = _height;

        IDXGIFactory6* factory = NULL;
        CreateDXGIFactory(__uuidof(IDXGIFactory6), (void**)&factory);

        if (enableDebugLayer) {
            ID3D12Debug1* debug;
            D3D12GetDebugInterface(IID_PPV_ARGS(&debug));
            debug->EnableDebugLayer();
            debug->Release();
        }

        selectAdapter(factory);
        createCommandQueues();

        DXGI_SWAP_CHAIN_DESC1 scDesc = {};
        initSwapChain(hwnd, factory, scDesc);

        srvHeap.init(device, 16384);

        factory->Release();

        device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
            IID_PPV_ARGS(&graphicsCommandAllocator[0]));
        device->CreateCommandList1(0, D3D12_COMMAND_LIST_TYPE_DIRECT, D3D12_COMMAND_LIST_FLAG_NONE,
            IID_PPV_ARGS(&graphicsCommandList[0]));

        device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
            IID_PPV_ARGS(&graphicsCommandAllocator[1]));
        device->CreateCommandList1(0, D3D12_COMMAND_LIST_TYPE_DIRECT, D3D12_COMMAND_LIST_FLAG_NONE,
            IID_PPV_ARGS(&graphicsCommandList[1]));

        D3D12_DESCRIPTOR_HEAP_DESC renderTargetViewHeapDesc = {};
        renderTargetViewHeapDesc.NumDescriptors = scDesc.BufferCount;
        renderTargetViewHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        device->CreateDescriptorHeap(&renderTargetViewHeapDesc, IID_PPV_ARGS(&backbufferHeap));

        backbuffers = new ID3D12Resource * [scDesc.BufferCount];

        D3D12_CPU_DESCRIPTOR_HANDLE renderTargetViewHandle = backbufferHeap->GetCPUDescriptorHandleForHeapStart();
        unsigned int renderTargetViewDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV); // size of one descriptor
        for (unsigned int i = 0; i < 2; i++) {
            swapchain->GetBuffer(i, IID_PPV_ARGS(&backbuffers[i]));
            device->CreateRenderTargetView(backbuffers[i], nullptr, renderTargetViewHandle);
            renderTargetViewHandle.ptr += renderTargetViewDescriptorSize;
        }

        graphicsQueueFence[0].create(device);
        graphicsQueueFence[1].create(device);

        D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc;
        memset(&dsvHeapDesc, 0, sizeof(D3D12_DESCRIPTOR_HEAP_DESC));
        dsvHeapDesc.NumDescriptors = 1;
        dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
        dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&dsvHeap));
        dsvHandle = dsvHeap->GetCPUDescriptorHandleForHeapStart();

        D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilDesc = {};
        depthStencilDesc.Format = DXGI_FORMAT_D32_FLOAT;
        depthStencilDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
        depthStencilDesc.Flags = D3D12_DSV_FLAG_NONE;
        D3D12_CLEAR_VALUE depthClearValue = {};
        depthClearValue.Format = DXGI_FORMAT_D32_FLOAT;
        depthClearValue.DepthStencil.Depth = 1.0f;
        depthClearValue.DepthStencil.Stencil = 0;

        D3D12_HEAP_PROPERTIES heapprops = {};
        heapprops.Type = D3D12_HEAP_TYPE_DEFAULT; // GPU local memory
        heapprops.CreationNodeMask = 1;
        heapprops.VisibleNodeMask = 1;

        D3D12_RESOURCE_DESC dsvDesc = {};
        dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
        dsvDesc.Width = wWidth;
        dsvDesc.Height = wHeight;
        dsvDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
        dsvDesc.DepthOrArraySize = 1;
        dsvDesc.MipLevels = 1;
        dsvDesc.SampleDesc.Count = 1;
        dsvDesc.SampleDesc.Quality = 0;
        dsvDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
        dsvDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;

        device->CreateCommittedResource(
            &heapprops, D3D12_HEAP_FLAG_NONE,
            &dsvDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE,
            &depthClearValue, IID_PPV_ARGS(&dsv)
        );

        device->CreateDepthStencilView(dsv, &depthStencilDesc, dsvHandle);

        viewport.TopLeftX = 0.0f;
        viewport.TopLeftY = 0.0f;
        viewport.Width = (float)wWidth;
        viewport.Height = (float)wHeight;
        viewport.MinDepth = 0.0f;
        viewport.MaxDepth = 1.0f;

        scissorRect.left = 0;
        scissorRect.top = 0;
        scissorRect.right = wWidth;
        scissorRect.bottom = wHeight;

        std::vector<D3D12_ROOT_PARAMETER> parameters;
        D3D12_ROOT_PARAMETER rootParameterCBVS;
        rootParameterCBVS.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
        rootParameterCBVS.Descriptor.ShaderRegister = 0; // Register(b0)
        rootParameterCBVS.Descriptor.RegisterSpace = 0;
        rootParameterCBVS.ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
        parameters.push_back(rootParameterCBVS);

        D3D12_ROOT_PARAMETER rootParameterCBPS;
        rootParameterCBPS.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
        rootParameterCBPS.Descriptor.ShaderRegister = 0; // Register(b0)
        rootParameterCBPS.Descriptor.RegisterSpace = 0;
        rootParameterCBPS.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
        parameters.push_back(rootParameterCBPS);

        D3D12_DESCRIPTOR_RANGE srvRange = {};
        srvRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
        srvRange.NumDescriptors = 8; // number of SRVs (t0–t7)
        srvRange.BaseShaderRegister = 0; // starting at t0
        srvRange.RegisterSpace = 0;
        srvRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
        D3D12_ROOT_PARAMETER rootParameterTex;
        rootParameterTex.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
        rootParameterTex.DescriptorTable.NumDescriptorRanges = 1;
        rootParameterTex.DescriptorTable.pDescriptorRanges = &srvRange;
        rootParameterTex.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
        parameters.push_back(rootParameterTex);

        D3D12_STATIC_SAMPLER_DESC staticSampler = {};
        staticSampler.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
        staticSampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        staticSampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        staticSampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        staticSampler.MipLODBias = 0;
        staticSampler.MaxAnisotropy = 1;
        staticSampler.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
        staticSampler.BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK;
        staticSampler.MinLOD = 0.0f;
        staticSampler.MaxLOD = D3D12_FLOAT32_MAX;
        staticSampler.ShaderRegister = 0;
        staticSampler.RegisterSpace = 0;
        staticSampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

        D3D12_ROOT_SIGNATURE_DESC desc = {};
        desc.NumParameters = parameters.size();
        desc.pParameters = &parameters[0];
        desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

        desc.NumStaticSamplers = 1;
        desc.pStaticSamplers = &staticSampler;

        ID3DBlob* serialized;
        ID3DBlob* error;
        D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1, &serialized, &error);
        device->CreateRootSignature(0, serialized->GetBufferPointer(), serialized->GetBufferSize(), IID_PPV_ARGS(&rootSignature));
        serialized->Release();

    }

    void resetCommandList() {
        unsigned int frameIndex = swapchain->GetCurrentBackBufferIndex(); // 0 or 1 depending on which buffer is being used
        graphicsCommandAllocator[frameIndex]->Reset();
        graphicsCommandList[frameIndex]->Reset(graphicsCommandAllocator[frameIndex], NULL);
    }

    ID3D12GraphicsCommandList4* getCommandList() {
        unsigned int frameIndex = swapchain->GetCurrentBackBufferIndex();
        return graphicsCommandList[frameIndex];
    }

    void runCommandList() {
        getCommandList()->Close();
        ID3D12CommandList* lists[] = { getCommandList() }; // executing one list at time to make it simple for now
        graphicsQueue->ExecuteCommandLists(1, lists);
    }

    void flushGraphicsQueue() {
        for (int i = 0; i < 2; i++) {
            graphicsQueueFence[i].signal(graphicsQueue);
            graphicsQueueFence[i].wait();
        }
    }

    void beginFrame() {
        unsigned int frameIndex = swapchain->GetCurrentBackBufferIndex();
        graphicsQueueFence[frameIndex].wait();

        D3D12_CPU_DESCRIPTOR_HANDLE renderTargetViewHandle = backbufferHeap->GetCPUDescriptorHandleForHeapStart();
        unsigned int renderTargetViewDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
        renderTargetViewHandle.ptr += frameIndex * renderTargetViewDescriptorSize;

        resetCommandList();

        Barrier::add(backbuffers[frameIndex], D3D12_RESOURCE_STATE_PRESENT,
            D3D12_RESOURCE_STATE_RENDER_TARGET, getCommandList());

        getCommandList()->OMSetRenderTargets(1, &renderTargetViewHandle, FALSE, &dsvHandle);
        float color[4];
        color[0] = 0;
        color[1] = 0;
        color[2] = 1.0;
        color[3] = 1.0;
        getCommandList()->ClearRenderTargetView(renderTargetViewHandle, color, 0, NULL);
        getCommandList()->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, NULL);
    }

    void finishFrame() {
        unsigned int frameIndex = swapchain->GetCurrentBackBufferIndex();
        Barrier::add(backbuffers[frameIndex], D3D12_RESOURCE_STATE_RENDER_TARGET,
            D3D12_RESOURCE_STATE_PRESENT, getCommandList());
        runCommandList();
        graphicsQueueFence[frameIndex].signal(graphicsQueue);
        swapchain->Present(1, 0);
    }

    // not so efficient. Better use copying queues, async work etc
    void uploadResource(ID3D12Resource* dstResource, const void* data, unsigned int size, D3D12_RESOURCE_STATES targetState, D3D12_PLACED_SUBRESOURCE_FOOTPRINT* texFootprint = NULL) {
        ID3D12Resource* uploadBuffer;
        D3D12_HEAP_PROPERTIES heapProps = {};
        heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;
        D3D12_RESOURCE_DESC bufferDesc = {};
        bufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
        bufferDesc.Width = size;
        bufferDesc.Height = 1; // we don't care about the height
        bufferDesc.DepthOrArraySize = 1;
        bufferDesc.MipLevels = 1;
        bufferDesc.SampleDesc.Count = 1;
        bufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
        device->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &bufferDesc,
            D3D12_RESOURCE_STATE_GENERIC_READ, NULL, IID_PPV_ARGS(&uploadBuffer));

        void* mappeddata = NULL;
        uploadBuffer->Map(0, NULL, &mappeddata); // Map makes the buffer CPU accessible
        memcpy(mappeddata, data, size);
        uploadBuffer->Unmap(0, NULL); // unmap when we finish copying data - we finished with the CPU pointer

        // Allocate commands to copy
        resetCommandList(); // reset the command list to record copy commands

        if (texFootprint != NULL) {
            D3D12_TEXTURE_COPY_LOCATION src = {};
            src.pResource = uploadBuffer;
            src.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
            src.PlacedFootprint = *texFootprint;
            D3D12_TEXTURE_COPY_LOCATION dst = {};
            dst.pResource = dstResource;
            dst.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
            dst.SubresourceIndex = 0;
            getCommandList()->CopyTextureRegion(&dst, 0, 0, 0, &src, NULL);
        }
        else {
            // if it isn't a texture, just do a buffer copy
            getCommandList()->CopyBufferRegion(dstResource, 0, uploadBuffer, 0, size);
        }

        Barrier::add(dstResource, D3D12_RESOURCE_STATE_COPY_DEST, targetState, getCommandList()); // prevent resource to be used before copy is finished

        runCommandList(); // execute the copy commands
        flushGraphicsQueue(); // wait until copy is finished. This is inefficient. In practice use multiple command queues and async work
        uploadBuffer->Release();
    }

    void beginRenderPass() {
        getCommandList()->SetDescriptorHeaps(1, &srvHeap.heap);
        getCommandList()->RSSetViewports(1, &viewport);
        getCommandList()->RSSetScissorRects(1, &scissorRect);
        getCommandList()->SetGraphicsRootSignature(rootSignature);
    }

};
