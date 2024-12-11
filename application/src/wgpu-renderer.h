#pragma once

#include <emscripten.h>
#include <webgpu/webgpu.h>
#include <glm/glm.hpp>


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

    void initRenderPipeline();

private:

    WGPUInstance mInstance;

    WGPUSurface mSurface;

    WGPUSwapChain mSwapChain;
    WGPUTextureFormat mSwapChainFormat = WGPUTextureFormat_BGRA8Unorm;

    WGPUDevice mDevice;

    WGPUQueue mQueue;

    // depth
    WGPUTexture mDepthDexture;
    WGPUTextureView mDepthTextureView;
    WGPUTextureFormat mDepthTextureFormat = WGPUTextureFormat_Depth24Plus;

    WGPUShaderModule mShaderModule;

    struct MyUniforms {
		// We add transform matrices
		glm::mat4x4 projectionMatrix;
		glm::mat4x4 viewMatrix;
		glm::mat4x4 modelMatrix;
		glm::vec4 color;
		float time;
		float _pad[3];
	};

    WGPUBindGroupLayout mBindGroupLayout;

    WGPURenderPipeline mPipeline;

};