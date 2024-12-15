#include "wgpu-renderer2.h"

#include "definitions.h"
#include "application.h"
#include "utils/glfw3webgpu.h"
#include "utils/webgpu-utils.h"
#include "utils/resource-manager.h"
#include "object.h"

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
}

void Renderer2::render(Scene& scene, LayerStack& layerStack)
{
    processCameraMovement();

    WGPUTextureView nextTexture = getNextSurfaceTextureView();
    if(!nextTexture)
        std::cerr << "[ERROR] Failed to acquire next swap chain texture" << '\n';
    
    WGPUCommandEncoder commandEncoder = wgpuDeviceCreateCommandEncoder(mDevice, nullptr);
    if(!commandEncoder)
        std::cerr << "[ERROR] Failed to create Command Encoder" << '\n';    

    WGPURenderPassDescriptor renderPassDesc{};

    WGPURenderPassColorAttachment renderPassColorAttachment{};
    renderPassColorAttachment.view = nextTexture;
    renderPassColorAttachment.loadOp = WGPULoadOp_Clear;
	renderPassColorAttachment.storeOp = WGPUStoreOp_Store;
    renderPassColorAttachment.clearValue = WGPUColor{ 0.3, 0.3, 1.0, 1.0 };
    renderPassColorAttachment.depthSlice = WGPU_DEPTH_SLICE_UNDEFINED;
	renderPassDesc.colorAttachmentCount = 1;
	renderPassDesc.colorAttachments = &renderPassColorAttachment;

    WGPURenderPassDepthStencilAttachment depthStencilAttachment{};
    depthStencilAttachment.view = mDepthTextureView;
	depthStencilAttachment.depthClearValue = 1.0f;
	depthStencilAttachment.depthLoadOp = WGPULoadOp_Clear;
	depthStencilAttachment.depthStoreOp = WGPUStoreOp_Store;
	depthStencilAttachment.depthReadOnly = false;
	depthStencilAttachment.stencilClearValue = 0;
	depthStencilAttachment.stencilLoadOp = WGPULoadOp_Undefined;
	depthStencilAttachment.stencilStoreOp = WGPUStoreOp_Undefined;
	depthStencilAttachment.stencilReadOnly = true;

    renderPassDesc.depthStencilAttachment = &depthStencilAttachment;

    WGPURenderPassEncoder renderPass = wgpuCommandEncoderBeginRenderPass(commandEncoder, &renderPassDesc);

    Camera& camera = Application::get().getMainCamera();

    // update camera
    glm::mat4 view(1.0f);
    view = camera.getViewMatrix();
    
    // draw scene
    // ----------
    for (Object& obj : scene.mObjects)
    {
        wgpuQueueWriteBuffer(mQueue, obj.getUniformBuffer(), offsetof(MyUniforms, viewMatrix), &view, sizeof(glm::mat4));

        wgpuRenderPassEncoderSetPipeline(renderPass, obj.getRenderPipeline());

        wgpuRenderPassEncoderSetBindGroup(renderPass, 0, obj.getBindGroup(), 0, nullptr);

        wgpuRenderPassEncoderSetVertexBuffer(renderPass, 0, obj.getVertexBuffer(), 0, wgpuBufferGetSize(obj.getVertexBuffer()));

        wgpuRenderPassEncoderDraw(renderPass, obj.getVertexCount(), 1, 0, 0);
    }

    // update layers
    // -------------
    for (Layer* layer : layerStack)
            layer->onUpdate(renderPass);

    

    wgpuRenderPassEncoderEnd(renderPass);
    wgpuRenderPassEncoderRelease(renderPass);

    WGPUCommandBuffer commandBuffer = wgpuCommandEncoderFinish(commandEncoder, nullptr);
    wgpuCommandEncoderRelease(commandEncoder);

    wgpuQueueSubmit(mQueue, 1, &commandBuffer);
    wgpuTextureViewRelease(nextTexture);

}

void Renderer2::renderScene(const Scene& scene, WGPURenderPassEncoder renderPass)
{
    
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
            renderer->processMouseMovement(xpos, ypos);
        }
    });
    glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, int mods)
    {
        Renderer2* renderer = static_cast<Renderer2*>(glfwGetWindowUserPointer(window));
        if (renderer) {
            renderer->processMouseButtonCallback(button, action, mods);
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

WGPUTextureView Renderer2::getNextSurfaceTextureView()
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

void Renderer2::processCameraMovement()
{
    float speed = 5.5f * Application::deltaTime;

    Application& app = Application::get();
    Camera& camera = app.getMainCamera();
    GLFWwindow* window = static_cast<GLFWwindow*>(app.getWindow().getNativeWindow());

    // Camera Movement
	if (glfwGetKey(window, GLFW_KEY_W) && GLFW_PRESS)
	{
		camera.mPosition += speed * camera.mFront;
	}
	if (glfwGetKey(window, GLFW_KEY_S) && GLFW_PRESS)
	{
		camera.mPosition -= speed * camera.mFront;
	}
	if (glfwGetKey(window, GLFW_KEY_A) && GLFW_PRESS)
	{
		camera.mPosition -= speed * glm::normalize(glm::cross(camera.mFront, camera.mUp));
	}
	if (glfwGetKey(window, GLFW_KEY_D) && GLFW_PRESS)
	{
		camera.mPosition += speed * glm::normalize(glm::cross(camera.mFront, camera.mUp));
	}
	if (glfwGetKey(window,GLFW_KEY_SPACE) && GLFW_PRESS)
	{
		camera.mPosition += speed * camera.mUp;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) && GLFW_PRESS)
	{
		camera.mPosition -= speed * camera.mUp;
	}
}

void Renderer2::processMouseMovement(double xposIn, double yposIn)
{
    Camera& camera = Application::get().getMainCamera();

    float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);
    if(camera.rightbuttonPressed)
    {

        if (camera.firstMouse)
        {
            camera.lastX = xpos;
            camera.lastY = ypos;
            camera.firstMouse = false;
        }

        float xoffset = xpos - camera.lastX;
        float yoffset = camera.lastY - ypos; // reversed since y-coordinates go from bottom to top
        camera.lastX = xpos;
        camera.lastY = ypos;

        float sensitivity = 0.1f; // change this value to your liking
        xoffset *= sensitivity;
        yoffset *= sensitivity;

        camera.yaw += xoffset;
        camera.pitch += yoffset;

        // make sure that when pitch is out of bounds, screen doesn't get flipped
        if (camera.pitch > 89.0f)
            camera.pitch = 89.0f;
        if (camera.pitch < -89.0f)
            camera.pitch = -89.0f;

        glm::vec3 front;
        front.x = cos(glm::radians(camera.yaw)) * cos(glm::radians(camera.pitch));
        front.y = sin(glm::radians(camera.pitch));
        front.z = sin(glm::radians(camera.yaw)) * cos(glm::radians(camera.pitch));
        camera.mFront = glm::normalize(front);
    }
}

void Renderer2::processMouseButtonCallback(int button, int action, int mods)
{
    Application& app = Application::get();
    GLFWwindow* window = static_cast<GLFWwindow*>(app.getWindow().getNativeWindow());
    Camera& camera = app.getMainCamera();

    if (button == GLFW_MOUSE_BUTTON_RIGHT) 
    {
        if (action == GLFW_PRESS) 
        {
            camera.rightbuttonPressed = true;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);  // Esconde o cursor
        }
        if (action == GLFW_RELEASE) 
        {
            camera.rightbuttonPressed = false;
            camera.firstMouse = true;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);  // Mostra o cursor
        }
    }
}
