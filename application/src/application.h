#pragma once

#include <emscripten.h>
#include <webgpu/webgpu.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

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

	void vertexBuffer();

	void deviceCapabilities(WGPUAdapter);

	WGPUTextureView getNextSurfaceTextureView();

private:

	GLFWwindow* mWindow;

	WGPUDevice mDevice;

	WGPUQueue mQueue;

	WGPUSurface mSurface;

	WGPUTextureFormat mSurfaceFormat = WGPUTextureFormat_BGRA8Unorm;

	WGPURenderPipeline mPipeline;

};