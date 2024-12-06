#pragma once

#include <emscripten.h>
#include <webgpu/webgpu.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <array>



class Application
{
public:

	void initialize();

	void terminate();

	void mainLoop();

	bool isRunning();

private:

	void initWindow();

	void initWebGPU();

private:

	void initializeRenderPipeline();

	void playingWithBuffers();

	void initializeBuffers();

	void initializeBindGroups();

	void deviceCapabilities(WGPUAdapter);

	WGPUTextureView getNextSurfaceTextureView();

private:

	GLFWwindow* mWindow;

	WGPUDevice mDevice;

	WGPUQueue mQueue;

	WGPUSurface mSurface;

	WGPUTextureFormat mSurfaceFormat = WGPUTextureFormat_BGRA8Unorm;

	WGPURenderPipeline mPipeline;

	WGPUPipelineLayout mLayout;

	WGPUBindGroupLayout mBindGroupLayout;

	WGPUBindGroup mBindGroup;

	WGPUBuffer vertexBuffer;
	uint32_t vertexCount;

	WGPUBuffer indexBuffer;
	uint32_t indexCount;

	WGPUBuffer uniformBuffer;

	struct MyUniforms {
		std::array<float, 4> color;
		float time;
		float _pad[3];
	};

	static_assert(sizeof(MyUniforms) % 16 == 0);

};