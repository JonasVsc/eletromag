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

    void initDepthBuffer();

private:

    WGPUInstance mInstance;

    WGPUSurface mSurface;

    WGPUSwapChain mSwapChain;

    WGPUDevice mDevice;

    WGPUQueue mQueue;

    // depth
    WGPUTexture mDepthDexture;
    WGPUTextureView mDepthTextureView;
};