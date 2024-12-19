#pragma once

#include "layer-stack.h"
#include "scene.h"

#include <emscripten.h>
#include <webgpu/webgpu.h>

#include <glm/glm.hpp>
#include <glm/ext.hpp>


class Renderer2
{

public:

    void init();

    void render(Scene* scene);

    void renderScene(const Scene& scene, WGPURenderPassEncoder renderPass);

    void terminate();

    static inline Renderer2& get() { return *sInstance; }

    inline WGPUDevice getDevice() { return mDevice; }

    inline WGPUQueue getQueue() { return mQueue; }
    
    inline WGPUTextureFormat getSwapChainFormat() { return mSwapChainFormat; }

    inline WGPUTextureFormat getDepthTextureFormat() { return mDepthTextureFormat; }

private:

    void initDevice();

    void initSwapChain();

    void initDepthBuffer();

    static WGPURequiredLimits checkAdapterCapabilities(WGPUAdapter adapter);

    WGPUTextureView getNextSurfaceTextureView();

    void processInput();

    void processMouseMovement(double xposIn, double yposIn);

    void processMouseScroll(double yoffset);

    void processMouseButtonCallback(int button, int action, int mods);

private:

    WGPUDevice mDevice;
    WGPUSurface mSurface;
    WGPUQueue mQueue;

    WGPUSwapChain mSwapChain;
    WGPUTextureFormat mSwapChainFormat = WGPUTextureFormat_BGRA8Unorm;

    WGPUTexture mDepthDexture;
    WGPUTextureView mDepthTextureView;
    WGPUTextureFormat mDepthTextureFormat = WGPUTextureFormat_Depth24Plus;

    static Renderer2* sInstance;
};