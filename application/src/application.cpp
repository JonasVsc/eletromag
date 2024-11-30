#include "application.h"

#include "webgpu-utils.h"
#include "glfw3webgpu.h"

#include<iostream>

void Application::initialize()
{
	initWindow();
	initWebGPU();
}

void Application::terminate()
{
	wgpuDeviceRelease(mDevice);
	wgpuQueueRelease(mQueue);

	glfwDestroyWindow(mWindow);
	glfwTerminate();
}

void Application::mainLoop()
{
	glfwPollEvents();
}

bool Application::isRunning()
{
	return !glfwWindowShouldClose(mWindow);
}

void Application::initWindow()
{
	if (!glfwInit())
	{
		throw std::runtime_error("failed glfw init");
	}

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	mWindow = glfwCreateWindow(640, 480, "Learn WebGPU", nullptr, nullptr);
}

void Application::initWebGPU()
{
	WGPUInstance instance = wgpuCreateInstance(nullptr);
	if (!instance)
		std::runtime_error("[ERROR] Could not initialize WebGPU!");
	std::cout << "[INFO] WGPU Instance: " << instance << std::endl;

	WGPUSurface surface = glfwGetWGPUSurface(instance, mWindow);

	std::cout << "[INFO] Requesting adapter..." << std::endl;
	WGPURequestAdapterOptions adapterOpts{};
	adapterOpts.nextInChain = nullptr;
	adapterOpts.compatibleSurface = surface;
	WGPUAdapter adapter = requestAdapterSync(instance, &adapterOpts);
	std::cout << "[INFO] Got Adapter: " << adapter << std::endl;

	wgpuSurfaceRelease(surface);
	wgpuInstanceRelease(instance);

	WGPUDeviceDescriptor deviceDescriptor{};
	deviceDescriptor.nextInChain = nullptr;
	deviceDescriptor.label = "My Device"; 
	deviceDescriptor.requiredFeatureCount = 0; 
	deviceDescriptor.requiredLimits = nullptr; 
	deviceDescriptor.defaultQueue.nextInChain = nullptr;
	deviceDescriptor.defaultQueue.label = "The default queue";
	deviceDescriptor.deviceLostCallback = nullptr;

	std::cout << "[INFO] Requesting device..." << std::endl;
	mDevice = requestDeviceSync(adapter, &deviceDescriptor);
	std::cout << "[INFO] Got device: " << mDevice << std::endl;

	deviceDescriptor.deviceLostCallback = [](WGPUDeviceLostReason reason, char const* message, void* userdata)
	{
		std::cout << "[WARN] Device lost: reason " << reason << '\n';
		if (message) std::cout << " (" << message << ')' << '\n';
	};

	auto onDeviceError = [](WGPUErrorType type, char const* message, void* /* pUserData */) 
	{
		std::cout << "[ERROR] Uncaptured device error: type " << type;
		if (message) std::cout << " (" << message << ")";
		std::cout << std::endl;
	};
	
	wgpuDeviceSetUncapturedErrorCallback(mDevice, onDeviceError, nullptr /* pUserData */);

	wgpuAdapterRelease(adapter);

	mQueue = wgpuDeviceGetQueue(mDevice);
}
