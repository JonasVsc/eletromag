#pragma once

#include <emscripten.h>
#include <webgpu/webgpu.h>

#include <glm/glm.hpp>
#include <glm/ext.hpp>

struct MyUniforms {
        glm::mat4x4 projectionMatrix;
        glm::mat4x4 viewMatrix;
        glm::mat4x4 modelMatrix;
		glm::vec4 color;
		float time;
		float _pad[3];
	};

static_assert(sizeof(MyUniforms) % 16 == 0);


class Renderer 
{
public:

    void init();


    void terminate();

    void render();

    inline WGPUQueue getQueue() { return mQueue; }

    inline WGPUBuffer getUniformBuffer() { return mUniformBuffer; }

private:

    void initDevice();

    void initSwapChain();

    void initDepthBuffer();

    void initRenderPipeline();

    void initTexture();

    void initGeometry();

    void initUniforms();

    void initBindGroup();

    // utility temporary
    WGPUTextureView getNextSurfaceTextureView();

    void processInput();

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

    WGPUBindGroupLayout mBindGroupLayout;

    WGPUBindGroup mBindGroup;

    WGPURenderPipeline mPipeline;

    WGPUTexture mTexture;
    WGPUTextureView mTextureView;

    WGPUSampler mSampler;

    WGPUBuffer mVertexBuffer;
    int mVertexCount;

    WGPUBuffer mIndexBuffer;
	uint32_t mIndexCount;


    WGPUBuffer mUniformBuffer;

    MyUniforms mUniforms;


};