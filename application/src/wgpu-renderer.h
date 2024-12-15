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
        glm::vec3 direction;
        float _pad0;
		float intensity;
        float mass;
		float _pad1[2];
};



static_assert(sizeof(MyUniforms) % 16 == 0);


class Renderer 
{
public:

    void init();

    void terminate();

    void render();

    inline WGPUQueue getQueue() { return mQueue; }

    inline WGPUBuffer getUniformBuffer() { return mUniformElectronBuffer; }
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

    glm::mat4 rotateToTarget(glm::mat4 modelMatrix, glm::vec3 currentPosition, glm::vec3 targetPosition);

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

    WGPURenderPipeline mPipeline;

    WGPUBindGroupLayout mBindGroupLayout;

    WGPUTexture mTexture;
    WGPUTextureView mTextureView;

    WGPUSampler mSampler;

    // Buffers
    WGPUBuffer mElectronVertexBuffer;
    int mElectronVertexCount;

    WGPUBuffer mFieldVertexBuffer;
    int mFieldVertexCount;

    WGPUBuffer mVectorVertexBuffer;
    int mVectorVertexCount;

    WGPUBuffer mUniformElectronBuffer;
    WGPUBuffer mUniformFieldBuffer;
    WGPUBuffer mUniformFMBuffer;

    WGPUBindGroup mElectronBindGroup;
    WGPUBindGroup mFieldBindGroup;
    WGPUBindGroup mVectorBindGroup;

    MyUniforms mUniforms;

    // external
    Camera mMainCamera;

    // deltaTime
    double deltaTime = 0.0f;
    double lastFrame = 0.0f;
};