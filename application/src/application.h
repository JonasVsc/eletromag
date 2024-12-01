#pragma once

#include <emscripten.h>

#include <webgpu/webgpu_cpp.h>
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
	void initializeRenderPipeline();

private:

	WGPUTextureView getNextSurfaceTextureView();

	GLFWwindow* mWindow;

	WGPUDevice mDevice;

	WGPUQueue mQueue;

	WGPUSurface mSurface;

	WGPUTextureFormat mSurfaceFormat = WGPUTextureFormat_BGRA8Unorm;

	WGPURenderPipeline mPipeline;

};