#include "wgpu-renderer.h"

#include "glfw3webgpu.h"
#include "webgpu-utils.h"
#include "app.h"

#include <iostream>
#include<cstdint>

////////////////////////////////////////////////////////////////////////////
// Public Methods //////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

void Renderer::init()
{
    initDevice();
    initSwapChain();
}


void Renderer::terminate()
{

    // terminate initDevice
    wgpuQueueRelease(mQueue);
    wgpuDeviceRelease(mDevice);
    wgpuSurfaceRelease(mSurface);
    wgpuInstanceRelease(mInstance);
}

void Renderer::update()
{

}

////////////////////////////////////////////////////////////////////////////
// Private Methods /////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

void Renderer::initDevice()
{
    mInstance = wgpuCreateInstance(nullptr);

    if(!mInstance)
        throw std::runtime_error("[ERROR] failed to create instance");

    Application& app = Application::get();
    GLFWwindow* window = static_cast<GLFWwindow*>(app.getWindow().getNativeWindow());

    mSurface = glfwGetWGPUSurface(mInstance, window);

    WGPURequestAdapterOptions adapterOpts{};
    adapterOpts.compatibleSurface = mSurface;

    WGPUAdapter adapter = requestAdapterSync(mInstance, &adapterOpts);
    std::cout << "[INFO] Adapter: " << adapter << std::endl;

    WGPUSupportedLimits supportedLimits{};
    wgpuAdapterGetLimits(adapter, &supportedLimits);
    
    WGPURequiredLimits requiredLimits{};
    requiredLimits.limits.maxVertexAttributes = 4;
	requiredLimits.limits.maxVertexBuffers = 1;
	requiredLimits.limits.maxBufferSize = 150000 * sizeof(WGPUVertexAttribute);
	requiredLimits.limits.maxVertexBufferArrayStride = sizeof(WGPUVertexAttribute);
	requiredLimits.limits.minStorageBufferOffsetAlignment = supportedLimits.limits.minStorageBufferOffsetAlignment;
	requiredLimits.limits.minUniformBufferOffsetAlignment = supportedLimits.limits.minUniformBufferOffsetAlignment;
	requiredLimits.limits.maxInterStageShaderComponents = 8;
	requiredLimits.limits.maxBindGroups = 1;
	requiredLimits.limits.maxUniformBuffersPerShaderStage = 1;
	requiredLimits.limits.maxUniformBufferBindingSize = 16 * 4 * sizeof(float);
	// Allow textures up to 2K
	requiredLimits.limits.maxTextureDimension1D = 2048;
	requiredLimits.limits.maxTextureDimension2D = 2048;
	requiredLimits.limits.maxTextureArrayLayers = 1;
	requiredLimits.limits.maxSampledTexturesPerShaderStage = 1;
	requiredLimits.limits.maxSamplersPerShaderStage = 1;

    WGPUDeviceDescriptor deviceDesc{};
    deviceDesc.label = "My Device";
    deviceDesc.requiredFeatureCount = 0;
	deviceDesc.requiredLimits = &requiredLimits;
	deviceDesc.defaultQueue.label = "The default queue";
    deviceDesc.deviceLostCallback = [](WGPUDeviceLostReason reason, char const* message, void* userdata)
	{
		std::cout << "[WARN] Device lost: reason " << reason << '\n';
		if (message) std::cout << " (" << message << ')' << '\n';
	};
    mDevice = requestDeviceSync(adapter, &deviceDesc);
    std::cout << "[INFO] Device: " << mDevice << std::endl;

    auto errorCallbackHandle = [](WGPUErrorType type, char const* message, void* /* pUserData */) 
	{
		std::cout << "[ERROR] Device error: type " << type;
		if (message) std::cout << " (" << message << ")";
		std::cout << std::endl;
	};
	wgpuDeviceSetUncapturedErrorCallback(mDevice, errorCallbackHandle, nullptr /* pUserData */);

    mQueue = wgpuDeviceGetQueue(mDevice);

    mSwapChainFormat = WGPUTextureFormat_BGRA8Unorm;

    wgpuAdapterRelease(adapter);
}

void Renderer::initSwapChain()
{
    Application& app = Application::get();
    GLFWwindow* window = static_cast<GLFWwindow*>(app.getWindow().getNativeWindow());
    // Get the current size of the window's framebuffer:
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);

	WGPUSwapChainDescriptor swapChainDesc;
	swapChainDesc.width = static_cast<uint32_t>(width);
	swapChainDesc.height = static_cast<uint32_t>(height);
	swapChainDesc.usage = WGPUTextureUsage_RenderAttachment;
	swapChainDesc.format = mSwapChainFormat;
	swapChainDesc.presentMode = WGPUPresentMode_Fifo;
	mSwapChain = wgpuDeviceCreateSwapChain(mDevice, mSurface, &swapChainDesc);
	std::cout << "Swapchain: " << mSwapChain << std::endl;

}
