#include "wgpu-renderer2.h"

#include "application.h"
#include "glfw3webgpu.h"
#include "webgpu-utils.h"
#include "resource-manager.h"



Renderer2* Renderer2::sInstance = nullptr;

////////////////////////////////////////////////////////////////////////////
// Public Methods //////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

void Renderer2::init()
{
    sInstance = this;

    initDevice();
    initSwapChain();
    initDepthBuffer();
    mPipeline.init();
}

void Renderer2::terminate()
{
    // terminate swapChain
    // -------------------
    wgpuSwapChainRelease(mSwapChain);

    // terminate device
    // ----------------
    wgpuQueueRelease(mQueue);
    wgpuDeviceRelease(mDevice);
    wgpuSurfaceRelease(mSurface);
}



////////////////////////////////////////////////////////////////////////////
// Private Methods /////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

void Renderer2::initDevice()
{
    WGPUInstance instance = wgpuCreateInstance(nullptr);

    if(!instance)
        std::cerr << "[ERROR] failed to create instance" << '\n';

    Application& app = Application::get();
    GLFWwindow* window = static_cast<GLFWwindow*>(app.getWindow().getNativeWindow());
    mSurface = glfwGetWGPUSurface(instance, window);

    glfwSetWindowUserPointer(window, this);

    // glfw callbacks
    // --------------
    glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xpos, double ypos)
    {
        Renderer2* renderer = static_cast<Renderer2*>(glfwGetWindowUserPointer(window));
        if (renderer) {
            // renderer->processMouseMovement(xpos, ypos);
        }
    });
    glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, int mods)
    {
        Renderer2* renderer = static_cast<Renderer2*>(glfwGetWindowUserPointer(window));
        if (renderer) {
            // renderer->processMouseButtonCallback(button, action, mods);
        }
    });

    WGPURequestAdapterOptions adapterOpts{};
    adapterOpts.compatibleSurface = mSurface;

    WGPUAdapter adapter = requestAdapterSync(instance, &adapterOpts);
    std::cout << "[INFO] Adapter: " << adapter << std::endl;
    wgpuInstanceRelease(instance);

    WGPURequiredLimits requiredLimits = checkAdapterCapabilities(adapter);

    WGPUDeviceDescriptor deviceDesc{};
    deviceDesc.requiredFeatureCount = 0;
	deviceDesc.requiredLimits = &requiredLimits;
	deviceDesc.defaultQueue.label = "defaultQueue";
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
    wgpuAdapterRelease(adapter);
}

void Renderer2::initSwapChain()
{
    Application& app = Application::get();
    GLFWwindow* window = static_cast<GLFWwindow*>(app.getWindow().getNativeWindow());
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);

	WGPUSwapChainDescriptor swapChainDesc;
	swapChainDesc.width = static_cast<uint32_t>(width);
	swapChainDesc.height = static_cast<uint32_t>(height);
	swapChainDesc.usage = WGPUTextureUsage_RenderAttachment;
	swapChainDesc.format = mSwapChainFormat;
	swapChainDesc.presentMode = WGPUPresentMode_Fifo;
	mSwapChain = wgpuDeviceCreateSwapChain(mDevice, mSurface, &swapChainDesc);
	std::cout << "[INFO] Swapchain: " << mSwapChain << std::endl;
}

void Renderer2::initDepthBuffer()
{
    WGPUTextureDescriptor depthTextureDesc{};
    depthTextureDesc.dimension = WGPUTextureDimension_2D;
    depthTextureDesc.format = mDepthTextureFormat;
    depthTextureDesc.mipLevelCount = 1;
    depthTextureDesc.sampleCount = 1;
    depthTextureDesc.size = { 1280, 720, 1 };
    depthTextureDesc.usage = WGPUTextureUsage_RenderAttachment;
    depthTextureDesc.viewFormatCount = 1;
    depthTextureDesc.viewFormats = &mDepthTextureFormat;
    mDepthDexture = wgpuDeviceCreateTexture(mDevice, &depthTextureDesc);
	std::cout << "[INFO] Depth texture: " << mDepthDexture << std::endl;

    WGPUTextureViewDescriptor depthTextureViewDesc{};
    depthTextureViewDesc.aspect = WGPUTextureAspect_DepthOnly;
	depthTextureViewDesc.baseArrayLayer = 0;
	depthTextureViewDesc.arrayLayerCount = 1;
	depthTextureViewDesc.baseMipLevel = 0;
	depthTextureViewDesc.mipLevelCount = 1;
	depthTextureViewDesc.dimension = WGPUTextureViewDimension_2D;
	depthTextureViewDesc.format = mDepthTextureFormat;
    mDepthTextureView = wgpuTextureCreateView(mDepthDexture, &depthTextureViewDesc);
	std::cout << "[INFO] Depth texture view: " << mDepthTextureView << std::endl;

    if(mDepthTextureView == nullptr)
        throw std::runtime_error("[ERROR] failed to create depth texture view");
}



////////////////////////////////////////////////////////////////////////////
// Utility Methods /////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////


WGPURequiredLimits Renderer2::checkAdapterCapabilities(WGPUAdapter adapter)
{
    WGPUSupportedLimits supportedLimits{};
    wgpuAdapterGetLimits(adapter, &supportedLimits);
    WGPURequiredLimits requiredLimits{};
    requiredLimits.limits.maxVertexAttributes = 4;
	requiredLimits.limits.maxVertexBuffers = 1;
	requiredLimits.limits.maxBufferSize = 10000 * sizeof(ResourceManager::VertexAttributes);
	requiredLimits.limits.maxVertexBufferArrayStride = sizeof(ResourceManager::VertexAttributes);
	requiredLimits.limits.minStorageBufferOffsetAlignment = supportedLimits.limits.minStorageBufferOffsetAlignment;
	requiredLimits.limits.minUniformBufferOffsetAlignment = supportedLimits.limits.minUniformBufferOffsetAlignment;
	requiredLimits.limits.maxInterStageShaderComponents = 8;
	requiredLimits.limits.maxBindGroups = 2;
	requiredLimits.limits.maxUniformBuffersPerShaderStage = 1;
	requiredLimits.limits.maxUniformBufferBindingSize = 16 * 4 * sizeof(float);
	requiredLimits.limits.maxTextureDimension1D = 2048;
	requiredLimits.limits.maxTextureDimension2D = 2048;
	requiredLimits.limits.maxTextureArrayLayers = 1;
	requiredLimits.limits.maxSampledTexturesPerShaderStage = 1;
	requiredLimits.limits.maxSamplersPerShaderStage = 1;
    return requiredLimits;
}

