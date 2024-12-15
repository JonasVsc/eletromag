#pragma once

#include "wgpu-pipeline.h"

#include <emscripten.h>
#include <webgpu/webgpu.h>

#include <glm/glm.hpp>
#include <glm/ext.hpp>

struct MyUniforms {
    glm::mat4x4 projectionMatrix;
    glm::mat4x4 viewMatrix;
    glm::mat4x4 modelMatrix;
    glm::vec4 color;
    glm::vec3 direction;
    float _pad0;
    float intensity;
    float mass;
    float _pad1[2];
};

static_assert(sizeof(MyUniforms) % 16 == 0);

class Renderer2
{

public:

    void init();

    void terminate();

    static inline Renderer2& get() { return *sInstance; }

    inline WGPUDevice getDevice() { return mDevice; }
    
    inline WGPUTextureFormat getSwapChainFormat() { return mSwapChainFormat; }

    inline WGPUTextureFormat getDepthTextureFormat() { return mDepthTextureFormat; }

private:

    void initDevice();

    void initSwapChain();

    void initDepthBuffer();

    // function checkDeviceCapabilities
    // input adapter
    // return WGPURequiredLimits

    static WGPURequiredLimits checkAdapterCapabilities(WGPUAdapter adapter);

private:

    WGPUDevice mDevice;
    WGPUSurface mSurface;
    WGPUQueue mQueue;

    WGPUSwapChain mSwapChain;
    WGPUTextureFormat mSwapChainFormat = WGPUTextureFormat_BGRA8Unorm;

    WGPUTexture mDepthDexture;
    WGPUTextureView mDepthTextureView;
    WGPUTextureFormat mDepthTextureFormat = WGPUTextureFormat_Depth24Plus;

    Pipeline mPipeline;

    static Renderer2* sInstance;

};