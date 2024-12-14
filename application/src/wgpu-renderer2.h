#pragma once

#include <emscripten.h>
#include <webgpu/webgpu.h>



class Renderer2
{

public:

    void init();

    void terminate();

private:

    void initDevice();

    // function checkDeviceCapabilities
    // input adapter
    // return WGPURequiredLimits

    static WGPURequiredLimits checkAdapterCapabilities(WGPUAdapter adapter);

private:

    WGPUDevice mDevice;
    WGPUSurface mSurface;
    WGPUQueue mQueue;

    // WGPUSwapChain mSwapChain;
    // WGPUTextureFormat mSwapChainFormat = WGPUTextureFormat_BGRA8Unorm;


    // Pipeline* mPipeline;

};