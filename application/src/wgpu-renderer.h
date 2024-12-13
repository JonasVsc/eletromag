#pragma once

#include "camera.h"

#include <emscripten.h>
#include <webgpu/webgpu.h>
#include "glfw3webgpu.h"


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

    void initGui();

    void initDevice();

    void initSwapChain();

    void initDepthBuffer();

    void initRenderPipeline();

    void initTexture();

    void initGeometry();

    void initUniforms();

    void initBindGroup();

    void updateGui(WGPURenderPassEncoder renderPass);

    // utility temporary
    WGPUTextureView getNextSurfaceTextureView();

    void framebufferSizeCallback(int newWidth, int newHeight);

    // events
    void processInput();

    // Mouse Movement
    void processMouseMovement(double xposIn, double yposIn);
    void processMouseButtonCallback(int button, int action, int mods);

	bool firstMouse = true;
    bool rightbuttonPressed = false;
	float yaw = -90.0f;
	float pitch = 0.0f;
	float lastX = 640.0f / 2.0;
	float lastY = 480.0 / 2.0;

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

    // external
    Camera mMainCamera;

    // deltaTime
    double deltaTime = 0.0f;
    double lastFrame = 0.0f;
};