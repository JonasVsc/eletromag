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
	wgpuSurfaceUnconfigure(mSurface);
	wgpuQueueRelease(mQueue);
	wgpuSurfaceRelease(mSurface);
	wgpuDeviceRelease(mDevice);
	glfwDestroyWindow(mWindow);
	glfwTerminate();
}

void Application::mainLoop()
{
	glfwPollEvents();
	
	WGPUTextureView targetView = getNextSurfaceTextureView();
	if (!targetView) return;

	// command encoder
	WGPUCommandEncoderDescriptor encoderDescriptor{};
	encoderDescriptor.nextInChain = nullptr;
	encoderDescriptor.label = "My command encoder";
	WGPUCommandEncoder encoder = wgpuDeviceCreateCommandEncoder(mDevice, &encoderDescriptor);

	WGPURenderPassDescriptor renderPassDescriptor{};
	renderPassDescriptor.nextInChain = nullptr;


	WGPURenderPassColorAttachment renderPassColorAttachment{};
	renderPassColorAttachment.view = targetView;
	renderPassColorAttachment.resolveTarget = nullptr;
	renderPassColorAttachment.loadOp = WGPULoadOp_Clear;
	renderPassColorAttachment.storeOp = WGPUStoreOp_Store;
	renderPassColorAttachment.clearValue = WGPUColor{ 0.9, 0.1, 0.2, 1.0 };
	renderPassColorAttachment.depthSlice = WGPU_DEPTH_SLICE_UNDEFINED;

	renderPassDescriptor.colorAttachmentCount = 1;
	renderPassDescriptor.colorAttachments = &renderPassColorAttachment;
	renderPassDescriptor.depthStencilAttachment = nullptr;
	renderPassDescriptor.timestampWrites = nullptr;

	WGPURenderPassEncoder renderPass = wgpuCommandEncoderBeginRenderPass(encoder, &renderPassDescriptor);
	wgpuRenderPassEncoderEnd(renderPass);
	wgpuRenderPassEncoderRelease(renderPass);

	WGPUCommandBufferDescriptor commandBufferDescriptor{};
	commandBufferDescriptor.nextInChain = nullptr;
	commandBufferDescriptor.label = "Command buffer";
	WGPUCommandBuffer commandBuffer = wgpuCommandEncoderFinish(encoder, &commandBufferDescriptor);
	wgpuCommandEncoderRelease(encoder);

	std::cout << "Submiting command..." << std::endl;
	wgpuQueueSubmit(mQueue, 1, &commandBuffer);
	wgpuCommandBufferRelease(commandBuffer);
	std::cout << "Command submitted." << std::endl;

	wgpuTextureViewRelease(targetView);
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


	std::cout << "[INFO] Requesting adapter..." << std::endl;
	mSurface = glfwGetWGPUSurface(instance, mWindow);
	WGPURequestAdapterOptions adapterOpts{};
	adapterOpts.nextInChain = nullptr;
	adapterOpts.compatibleSurface = mSurface;
	WGPUAdapter adapter = requestAdapterSync(instance, &adapterOpts);
	std::cout << "[INFO] Got Adapter: " << adapter << std::endl;

	wgpuInstanceRelease(instance);

	std::cout << "[INFO] Requesting device..." << std::endl;
	WGPUDeviceDescriptor deviceDescriptor{};
	deviceDescriptor.nextInChain = nullptr;
	deviceDescriptor.label = "My Device"; 
	deviceDescriptor.requiredFeatureCount = 0; 
	deviceDescriptor.requiredLimits = nullptr; 
	deviceDescriptor.defaultQueue.nextInChain = nullptr;
	deviceDescriptor.defaultQueue.label = "The default queue";
	deviceDescriptor.deviceLostCallback = [](WGPUDeviceLostReason reason, char const* message, void* userdata)
	{
		std::cout << "[WARN] Device lost: reason " << reason << '\n';
		if (message) std::cout << " (" << message << ')' << '\n';
	};
	mDevice = requestDeviceSync(adapter, &deviceDescriptor);
	std::cout << "[INFO] Got device: " << mDevice << std::endl;
	auto onDeviceError = [](WGPUErrorType type, char const* message, void* /* pUserData */) 
	{
		std::cout << "[ERROR] Uncaptured device error: type " << type;
		if (message) std::cout << " (" << message << ")";
		std::cout << std::endl;
	};
	
	wgpuDeviceSetUncapturedErrorCallback(mDevice, onDeviceError, nullptr /* pUserData */);

	mQueue = wgpuDeviceGetQueue(mDevice);



	WGPUSurfaceConfiguration surfaceConfig{};
	surfaceConfig.nextInChain = nullptr;

	surfaceConfig.width = 640;
	surfaceConfig.height = 480;
	surfaceConfig.usage = WGPUTextureUsage_RenderAttachment;
	WGPUTextureFormat surfaceFormat = wgpuSurfaceGetPreferredFormat(mSurface, adapter);
	surfaceConfig.format = surfaceFormat;

	surfaceConfig.viewFormatCount = 0;
	surfaceConfig.viewFormats = nullptr;
	surfaceConfig.device = mDevice;
	surfaceConfig.presentMode = WGPUPresentMode_Fifo;
	surfaceConfig.alphaMode = WGPUCompositeAlphaMode_Auto;

	wgpuSurfaceConfigure(mSurface, &surfaceConfig);

	wgpuAdapterRelease(adapter);

}

WGPUTextureView Application::getNextSurfaceTextureView()
{
	WGPUSurfaceTexture surfaceTexture;
	wgpuSurfaceGetCurrentTexture(mSurface, &surfaceTexture);
	if (surfaceTexture.status != WGPUSurfaceGetCurrentTextureStatus_Success)
		return nullptr;

	WGPUTextureViewDescriptor viewDescriptor;
	viewDescriptor.nextInChain = nullptr;
	viewDescriptor.label = "Surface texture view";
	viewDescriptor.format = wgpuTextureGetFormat(surfaceTexture.texture);
	viewDescriptor.dimension = WGPUTextureViewDimension_2D;
	viewDescriptor.baseMipLevel = 0;
	viewDescriptor.mipLevelCount = 1;
	viewDescriptor.baseArrayLayer = 0;
	viewDescriptor.arrayLayerCount = 1;
	viewDescriptor.aspect = WGPUTextureAspect_All;
	WGPUTextureView targetView = wgpuTextureCreateView(surfaceTexture.texture, &viewDescriptor);

	wgpuTextureRelease(surfaceTexture.texture);

	return targetView;
}
