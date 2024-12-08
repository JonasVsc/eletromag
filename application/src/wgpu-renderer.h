#pragma once

#include <emscripten.h>
#include <webgpu/webgpu.h>


class Renderer 
{
public:

    void init();

    void update();

    void terminate();

private:

    void initDevice();

    void initSwapChain();

private:

    WGPUInstance mInstance;

    WGPUSurface mSurface;

	WGPUTextureFormat mSurfaceFormat = WGPUTextureFormat_BGRA8Unorm;

    WGPUSwapChain mSwapChain;

    WGPUTextureFormat mSwapChainFormat;

    WGPUDevice mDevice;

    WGPUQueue mQueue;
};