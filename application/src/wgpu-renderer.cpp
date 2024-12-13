#include "wgpu-renderer.h"

#include "glfw3webgpu.h"
#include "webgpu-utils.h"
#include "application.h"
#include "resource-manager.h"
#include "window.h"

#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <iostream>
#include<cstdint>

#include <imgui.h>
#include <backends/imgui_impl_wgpu.h>
#include <backends/imgui_impl_glfw.h>

constexpr float PI = 3.14159265358979323846f;

////////////////////////////////////////////////////////////////////////////
// Public Methods //////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

void Renderer::init()
{
    initDevice();
    initSwapChain();
    initDepthBuffer();
    initRenderPipeline();
    initGeometry();
    initUniforms();
    initBindGroup();
    initGui();
}

void Renderer::render()
{
    // delta Time
    deltaTime = glfwGetTime() - lastFrame;
	lastFrame = glfwGetTime();

    processInput();

    // update camera
    glm::mat4 view(1.0f);
    view = mMainCamera.getViewMatrix();
    wgpuQueueWriteBuffer(mQueue, mUniformElectronBuffer, offsetof(MyUniforms, viewMatrix), &view, sizeof(glm::mat4));
    wgpuQueueWriteBuffer(mQueue, mUniformFieldBuffer, offsetof(MyUniforms, viewMatrix), &view, sizeof(glm::mat4));
    wgpuQueueWriteBuffer(mQueue, mUniformVectorBuffer, offsetof(MyUniforms, viewMatrix), &view, sizeof(glm::mat4));


    WGPUTextureView nextTexture = getNextSurfaceTextureView();
    if(!nextTexture)
        throw std::runtime_error("failed to acquire next swap chain texture");

    // command encoder
	WGPUCommandEncoderDescriptor encoderDescriptor = {};
	encoderDescriptor.nextInChain = nullptr;
	encoderDescriptor.label = "Command Encoder";
	WGPUCommandEncoder commandEncoder = wgpuDeviceCreateCommandEncoder(mDevice, &encoderDescriptor);

	WGPURenderPassDescriptor renderPassDesc{};

    WGPURenderPassColorAttachment renderPassColorAttachment{};
    renderPassColorAttachment.view = nextTexture;
    renderPassColorAttachment.resolveTarget = nullptr;
    renderPassColorAttachment.loadOp = WGPULoadOp_Clear;
	renderPassColorAttachment.storeOp = WGPUStoreOp_Store;
    renderPassColorAttachment.clearValue = WGPUColor{ 1.0, 1.0, 1.0, 1.0 };
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
    renderPassDesc.timestampWrites = nullptr;

	WGPURenderPassEncoder renderPass = wgpuCommandEncoderBeginRenderPass(commandEncoder, &renderPassDesc);
	
    wgpuRenderPassEncoderSetPipeline(renderPass, mPipeline);

    // electron 
    wgpuRenderPassEncoderSetVertexBuffer(renderPass, 0, mElectronVertexBuffer, 0, wgpuBufferGetSize(mElectronVertexBuffer));
    wgpuRenderPassEncoderSetBindGroup(renderPass, 0, mElectronBindGroup, 0, nullptr);
	wgpuRenderPassEncoderDraw(renderPass, mElectronVertexCount, 1, 0, 0);

    // field
    wgpuRenderPassEncoderSetVertexBuffer(renderPass, 0, mFieldVertexBuffer, 0, wgpuBufferGetSize(mFieldVertexBuffer));
    wgpuRenderPassEncoderSetBindGroup(renderPass, 0, mFieldBindGroup, 0, nullptr);
    wgpuRenderPassEncoderDraw(renderPass, mFieldVertexCount, 1, 0, 0);

    // vector
    wgpuRenderPassEncoderSetVertexBuffer(renderPass, 0, mVectorVertexBuffer, 0, wgpuBufferGetSize(mVectorVertexBuffer));
    wgpuRenderPassEncoderSetBindGroup(renderPass, 0, mVectorBindGroup, 0, nullptr);
    wgpuRenderPassEncoderDraw(renderPass, mVectorVertexCount, 1, 0, 0);
    
    
    updateGui(renderPass);

    wgpuRenderPassEncoderEnd(renderPass);
    wgpuRenderPassEncoderRelease(renderPass);

    WGPUCommandBufferDescriptor commandBufferDesc{};
    commandBufferDesc.label = "Command Buffer";
    WGPUCommandBuffer commandBuffer = wgpuCommandEncoderFinish(commandEncoder, &commandBufferDesc);
	wgpuCommandEncoderRelease(commandEncoder);

    wgpuQueueSubmit(mQueue, 1, &commandBuffer);
    wgpuCommandBufferRelease(commandBuffer);

    wgpuTextureViewRelease(nextTexture);
}



void Renderer::terminate()
{
    // terminate gui
    ImGui_ImplGlfw_Shutdown();
    ImGui_ImplWGPU_Shutdown();

    // terminate bind groups
    wgpuBindGroupRelease(mElectronBindGroup);
    wgpuBindGroupRelease(mFieldBindGroup);
    wgpuBindGroupRelease(mVectorBindGroup);

    // terminate uniforms
    wgpuBufferDestroy(mUniformElectronBuffer);
    wgpuBufferRelease(mUniformElectronBuffer);
    wgpuBufferDestroy(mUniformFieldBuffer);
    wgpuBufferRelease(mUniformFieldBuffer);
    wgpuBufferDestroy(mUniformVectorBuffer);
    wgpuBufferRelease(mUniformVectorBuffer);

    // terminate geometry
    wgpuBufferDestroy(mElectronVertexBuffer);
    wgpuBufferRelease(mElectronVertexBuffer);
    mElectronVertexCount = 0;

    wgpuBufferDestroy(mFieldVertexBuffer);
    wgpuBufferRelease(mFieldVertexBuffer);
    mFieldVertexCount = 0;

    wgpuBufferDestroy(mVectorVertexBuffer);
    wgpuBufferRelease(mVectorVertexBuffer);
    mVectorVertexCount = 0;

    // terminate render pipeline
    wgpuRenderPipelineRelease(mPipeline);
    wgpuShaderModuleRelease(mShaderModule);
    wgpuBindGroupLayoutRelease(mBindGroupLayout);

    // terminate depthBuffer
    wgpuTextureViewRelease(mDepthTextureView);
    wgpuTextureDestroy(mDepthDexture);
    wgpuTextureRelease(mDepthDexture);

    // terminate swapChain
    wgpuSwapChainRelease(mSwapChain);

    // terminate initDevice
    wgpuQueueRelease(mQueue);
    wgpuDeviceRelease(mDevice);
    wgpuSurfaceRelease(mSurface);
    wgpuInstanceRelease(mInstance);
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

    glfwSetWindowUserPointer(window, this);
    
    // callbacks 
    glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xpos, double ypos)
    {
        Renderer* renderer = static_cast<Renderer*>(glfwGetWindowUserPointer(window));
        if (renderer) {
            renderer->processMouseMovement(xpos, ypos);
        }
    });

    glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, int mods)
    {
        Renderer* renderer = static_cast<Renderer*>(glfwGetWindowUserPointer(window));
        if (renderer) {
            renderer->processMouseButtonCallback(button, action, mods);
        }
    });

    WGPURequestAdapterOptions adapterOpts{};
    adapterOpts.compatibleSurface = mSurface;

    WGPUAdapter adapter = requestAdapterSync(mInstance, &adapterOpts);
    std::cout << "[INFO] Adapter: " << adapter << std::endl;

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
	std::cout << "[INFO] Swapchain: " << mSwapChain << std::endl;
}

void Renderer::initDepthBuffer()
{
    // in case of bug, try make this a member

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

void Renderer::initRenderPipeline()
{
    mShaderModule = ResourceManager::loadShaderModule("C:/Dev/eletromag/application/resources/shader.wgsl", mDevice);
    if(mShaderModule == nullptr)
        throw std::runtime_error("failed to load shader module.");

    WGPURenderPipelineDescriptor pipelineDesc{};

    std::vector<WGPUVertexAttribute> vertexAttribs(3);

    // Position attribute
    vertexAttribs[0].shaderLocation = 0;
    vertexAttribs[0].format = WGPUVertexFormat_Float32x3;
    vertexAttribs[0].offset = offsetof(ResourceManager::VertexAttributes, position);

    vertexAttribs[1].shaderLocation = 1;
    vertexAttribs[1].format = WGPUVertexFormat_Float32x3;
	vertexAttribs[1].offset = offsetof(ResourceManager::VertexAttributes, normal);
    
    vertexAttribs[2].shaderLocation = 2;
    vertexAttribs[2].format = WGPUVertexFormat_Float32x3;
	vertexAttribs[2].offset = offsetof(ResourceManager::VertexAttributes, color);

    WGPUVertexBufferLayout vertexBufferLayout{};
    vertexBufferLayout.attributeCount = (uint32_t)vertexAttribs.size();
    vertexBufferLayout.attributes = vertexAttribs.data();
    vertexBufferLayout.arrayStride = sizeof(ResourceManager::VertexAttributes);
    vertexBufferLayout.stepMode = WGPUVertexStepMode_Vertex;

    pipelineDesc.vertex.bufferCount = 1;
    pipelineDesc.vertex.buffers = &vertexBufferLayout;
    pipelineDesc.vertex.module = mShaderModule;
    pipelineDesc.vertex.entryPoint = "vs_main";
    pipelineDesc.vertex.constantCount = 0;
    pipelineDesc.vertex.constants = nullptr;

    pipelineDesc.primitive.topology = WGPUPrimitiveTopology_TriangleList;
    pipelineDesc.primitive.stripIndexFormat = WGPUIndexFormat_Undefined;
    pipelineDesc.primitive.frontFace = WGPUFrontFace_CCW;
    pipelineDesc.primitive.cullMode = WGPUCullMode_Undefined;

    WGPUFragmentState fragmentState{};
    fragmentState.module = mShaderModule;
    fragmentState.entryPoint = "fs_main";
    fragmentState.constantCount = 0;
    fragmentState.constants = nullptr;
    pipelineDesc.fragment = &fragmentState;

    WGPUBlendState blendState{};
    blendState.color.srcFactor = WGPUBlendFactor_SrcAlpha;
	blendState.color.dstFactor = WGPUBlendFactor_OneMinusSrcAlpha;
	blendState.color.operation = WGPUBlendOperation_Add;
	blendState.alpha.srcFactor = WGPUBlendFactor_Zero;
	blendState.alpha.dstFactor = WGPUBlendFactor_One;
	blendState.alpha.operation = WGPUBlendOperation_Add;

    WGPUColorTargetState colorTarget{};
    colorTarget.format = mSwapChainFormat;
    colorTarget.blend = &blendState;
    colorTarget.writeMask = WGPUColorWriteMask_All;

    fragmentState.targetCount = 1;
    fragmentState.targets = &colorTarget;
    
    WGPUDepthStencilState depthStencilState{};
    depthStencilState.format = mDepthTextureFormat;
    depthStencilState.depthWriteEnabled = true;
    depthStencilState.depthCompare = WGPUCompareFunction_Less;
    depthStencilState.stencilReadMask = 0;
    depthStencilState.stencilWriteMask = 0;
    depthStencilState.depthBias = 0;
    depthStencilState.depthBiasSlopeScale = 0;
    depthStencilState.depthBiasClamp = 0;

    depthStencilState.stencilFront.compare = WGPUCompareFunction_Always;
    depthStencilState.stencilFront.failOp = WGPUStencilOperation_Keep;
    depthStencilState.stencilFront.depthFailOp = WGPUStencilOperation_Keep;
    depthStencilState.stencilFront.passOp = WGPUStencilOperation_Keep;
	
	depthStencilState.stencilBack.compare = WGPUCompareFunction_Always;
    depthStencilState.stencilBack.failOp = WGPUStencilOperation_Keep;
    depthStencilState.stencilBack.depthFailOp = WGPUStencilOperation_Keep;
    depthStencilState.stencilBack.passOp = WGPUStencilOperation_Keep;
	
    pipelineDesc.depthStencil = &depthStencilState;

    pipelineDesc.multisample.count = 1;
    pipelineDesc.multisample.mask = ~0u;
    pipelineDesc.multisample.alphaToCoverageEnabled = false;


    // uniform buffer binding
	WGPUBindGroupLayoutEntry bindingLayout{};
    bindingLayout.binding = 0;
    bindingLayout.visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment;
    bindingLayout.buffer.type = WGPUBufferBindingType_Uniform;
    bindingLayout.buffer.minBindingSize = sizeof(MyUniforms);

    // bind group layout
    WGPUBindGroupLayoutDescriptor bindGroupLayoutDesc{};
    bindGroupLayoutDesc.entryCount = 1;
    bindGroupLayoutDesc.entries = &bindingLayout;
    mBindGroupLayout = wgpuDeviceCreateBindGroupLayout(mDevice, &bindGroupLayoutDesc);

    // pipeline layout
    WGPUPipelineLayoutDescriptor layoutDesc{};
    layoutDesc.bindGroupLayoutCount = 1;
    layoutDesc.bindGroupLayouts = &mBindGroupLayout;
    WGPUPipelineLayout layout = wgpuDeviceCreatePipelineLayout(mDevice, &layoutDesc);
    pipelineDesc.layout = layout;

    mPipeline = wgpuDeviceCreateRenderPipeline(mDevice, &pipelineDesc);
    std::cout << "[INFO] Render pipeline: " << mPipeline << std::endl;

    if(mPipeline == nullptr)
        std::cerr << "failed create render pipeline" << '\n';
}

void Renderer::initGeometry()
{
    std::vector<ResourceManager::VertexAttributes> vertexData;

	bool success = ResourceManager::loadGeometryFromObj("C:/Dev/eletromag/application/resources/electron.obj", vertexData);
    if(!success)
        std::cerr << "[ERROR] could not load geometry" << '\n';

    mElectronVertexCount = static_cast<uint32_t>(vertexData.size());

	// electron vertex buffer
    WGPUBufferDescriptor bufferDesc{};
	bufferDesc.label = "electron vertex buffer";
    bufferDesc.size = vertexData.size() * sizeof(ResourceManager::VertexAttributes);
	bufferDesc.usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Vertex;
	bufferDesc.mappedAtCreation = false;
    mElectronVertexBuffer = wgpuDeviceCreateBuffer(mDevice, &bufferDesc);
	wgpuQueueWriteBuffer(mQueue, mElectronVertexBuffer, 0, vertexData.data(), bufferDesc.size);
    vertexData.clear();

    // field vertex buffer
    success = ResourceManager::loadGeometryFromObj("C:/Dev/eletromag/application/resources/field.obj", vertexData);
    if(!success)
        std::cerr << "[ERROR] could not load geometry" << '\n';

    mFieldVertexCount = static_cast<uint32_t>(vertexData.size());

    bufferDesc.label = "field vertex buffer";
    bufferDesc.size = vertexData.size() * sizeof(ResourceManager::VertexAttributes);
    mFieldVertexBuffer = wgpuDeviceCreateBuffer(mDevice, &bufferDesc);
    wgpuQueueWriteBuffer(mQueue, mFieldVertexBuffer, 0, vertexData.data(), bufferDesc.size);
    vertexData.clear();

    // vector vertex buffer
    success = ResourceManager::loadGeometryFromObj("C:/Dev/eletromag/application/resources/vector.obj", vertexData);
    if(!success)
        std::cerr << "[ERROR] could not load geometry" << '\n';

    mVectorVertexCount = static_cast<uint32_t>(vertexData.size());

    bufferDesc.label = "vector vertex buffer";
    bufferDesc.size = vertexData.size() * sizeof(ResourceManager::VertexAttributes);
    mVectorVertexBuffer = wgpuDeviceCreateBuffer(mDevice, &bufferDesc);
    wgpuQueueWriteBuffer(mQueue, mVectorVertexBuffer, 0, vertexData.data(), bufferDesc.size);

}

void Renderer::initUniforms()
{

    // Create electron uniform buffer
	WGPUBufferDescriptor bufferDesc{};
	bufferDesc.size = sizeof(MyUniforms);
	bufferDesc.usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform;
	bufferDesc.mappedAtCreation = false;
	mUniformElectronBuffer = wgpuDeviceCreateBuffer(mDevice, &bufferDesc);
	mUniformFieldBuffer = wgpuDeviceCreateBuffer(mDevice, &bufferDesc);
	mUniformVectorBuffer = wgpuDeviceCreateBuffer(mDevice, &bufferDesc);

    mUniforms.modelMatrix = glm::mat4x4(1.0f);
    mUniforms.viewMatrix = mMainCamera.getViewMatrix();
    mUniforms.projectionMatrix = glm::perspective(45 * PI / 180, 640.0f / 480.0f, 0.01f, 100.0f);
    mUniforms.time = 1.0f;
	mUniforms.color = { 0.0f, 0.0f, 1.0f, 1.0f };


    wgpuQueueWriteBuffer(mQueue, mUniformElectronBuffer, 0, &mUniforms, sizeof(MyUniforms));

	mUniforms.color = { 1.0f, 0.0f, 0.0f, 1.0f };
    wgpuQueueWriteBuffer(mQueue, mUniformFieldBuffer, 0, &mUniforms, sizeof(MyUniforms));

    mUniforms.color = { 0.0f, 1.0f, 0.0f, 1.0f };
    wgpuQueueWriteBuffer(mQueue, mUniformVectorBuffer, 0, &mUniforms, sizeof(MyUniforms));

    if(mUniformElectronBuffer == nullptr || mUniformVectorBuffer == nullptr || mUniformFieldBuffer == nullptr)
        std::cerr << "failed to init uniform buffer" << '\n';
}

void Renderer::initBindGroup()
{
    // electron bind group
    WGPUBindGroupEntry bindings{};
    bindings.binding = 0;
    bindings.buffer = mUniformElectronBuffer;
    bindings.offset = 0;
    bindings.size = sizeof(MyUniforms);

    WGPUBindGroupDescriptor bindGroupDesc{};
    bindGroupDesc.layout = mBindGroupLayout;
    bindGroupDesc.entryCount = 1;
    bindGroupDesc.entries = &bindings;
    mElectronBindGroup = wgpuDeviceCreateBindGroup(mDevice, &bindGroupDesc);

    bindings.buffer = mUniformFieldBuffer;
    bindGroupDesc.entries = &bindings;
    mFieldBindGroup = wgpuDeviceCreateBindGroup(mDevice, &bindGroupDesc);

    bindings.buffer = mUniformVectorBuffer;
    bindGroupDesc.entries = &bindings;
    mVectorBindGroup = wgpuDeviceCreateBindGroup(mDevice, &bindGroupDesc);

    if(mUniformElectronBuffer == nullptr || mVectorBindGroup == nullptr || mFieldBindGroup == nullptr)
        std::cerr << "failed to init bind group" << '\n';
}

void Renderer::initGui()
{
    Application& app = Application::get();
    GLFWwindow* window = static_cast<GLFWwindow*>(app.getWindow().getNativeWindow());
     // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::GetIO();
    ImGui::StyleColorsDark();
    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOther(window, true);
    ImGui_ImplWGPU_InitInfo init_info;
    init_info.Device = mDevice;
    init_info.NumFramesInFlight = 3;
    init_info.RenderTargetFormat = WGPUTextureFormat_BGRA8Unorm;
    init_info.DepthStencilFormat = WGPUTextureFormat_Depth24Plus;
    ImGui_ImplWGPU_Init(&init_info);
}

void Renderer::updateGui(WGPURenderPassEncoder renderPass)
{
    // Start the Dear ImGui frame
    ImGui_ImplWGPU_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // aux data
    static float electronPosition[3] = {0.0f, 0.0f, 0.0f};
    static float electronRotation[3] = {0.0f, 0.0f, 0.0f};
    static float electronScale[3] = {0.1f, 0.1f, 0.1f};

    static float fieldPosition[3] = {0.0f, 0.0f, 0.0f};
    static float fieldRotation[3] = {0.0f, 0.0f, 0.0f};
    static float fieldScale[3] = {0.1f, 0.1f, 0.1f};

    static float vectorPosition[3] = {0.0f, 0.0f, 0.0f};
    static float vectorRotation[3] = {0.0f, 0.0f, 0.0f};
    static float vectorScale[3] = {0.1f, 0.1f, 0.1f};

    ImGui::Begin("Objetos Cena");                                

    ImGui::Text("Carga: ");
    // inputs
    ImGui::DragFloat3("Position##1", electronPosition, 0.1f, -100.0f, 100.0f);
    ImGui::DragFloat3("Rotation##1", electronRotation, 0.1f, -360.0f, 360.0f);
    ImGui::DragFloat3("Scale##1", electronScale, 0.1f, -360.0f, 360.0f);

    // buffer update
    glm::mat4 electronModel(1.0f);
    electronModel = glm::translate(electronModel, glm::vec3(electronPosition[0], electronPosition[1], electronPosition[2]));
    electronModel = glm::rotate(electronModel, glm::radians(electronRotation[0]), glm::vec3(1.0f, 0.0f, 0.0f));
    electronModel = glm::rotate(electronModel, glm::radians(electronRotation[1]), glm::vec3(0.0f, 1.0f, 0.0f));
    electronModel = glm::rotate(electronModel, glm::radians(electronRotation[2]), glm::vec3(0.0f, 0.0f, 1.0f));
    electronModel = glm::scale(electronModel, glm::vec3(electronScale[0], electronScale[1], electronScale[2]));
    wgpuQueueWriteBuffer(mQueue, mUniformElectronBuffer, offsetof(MyUniforms, modelMatrix), &electronModel, sizeof(glm::mat4));

    ImGui::Text("Campo Magnético: ");
    // inputs
    ImGui::DragFloat3("Position##2", fieldPosition, 0.1f, -100.0f, 100.0f);
    ImGui::DragFloat3("Rotation##2", fieldRotation, 0.1f, -360.0f, 360.0f);
    ImGui::DragFloat3("Scale##2", fieldScale, 0.1f, -360.0f, 360.0f);

    // buffer update
    glm::mat4 fieldModel(1.0f);
    fieldModel = glm::translate(fieldModel, glm::vec3(fieldPosition[0], fieldPosition[1], fieldPosition[2]));
    fieldModel = glm::rotate(fieldModel, glm::radians(fieldRotation[0]), glm::vec3(1.0f, 0.0f, 0.0f));
    fieldModel = glm::rotate(fieldModel, glm::radians(fieldRotation[1]), glm::vec3(0.0f, 1.0f, 0.0f));
    fieldModel = glm::rotate(fieldModel, glm::radians(fieldRotation[2]), glm::vec3(0.0f, 0.0f, 1.0f));
    fieldModel = glm::scale(fieldModel, glm::vec3(fieldScale[0], fieldScale[1], fieldScale[2]));
    wgpuQueueWriteBuffer(mQueue, mUniformFieldBuffer, offsetof(MyUniforms, modelMatrix), &fieldModel, sizeof(glm::mat4));

    ImGui::Text("Força Magnética: ");
    // inputs
    ImGui::DragFloat3("Position##3", vectorPosition, 0.1f, -100.0f, 100.0f);
    ImGui::DragFloat3("Rotation##3", vectorRotation, 0.1f, -360.0f, 360.0f);
    ImGui::DragFloat3("Scale##3", vectorScale, 0.1f, -360.0f, 360.0f);

    // buffer update
    glm::mat4 vectorModel(1.0f);
    vectorModel = glm::translate(vectorModel, glm::vec3(vectorPosition[0], vectorPosition[1], vectorPosition[2]));
    vectorModel = glm::rotate(vectorModel, glm::radians(vectorRotation[0]), glm::vec3(1.0f, 0.0f, 0.0f));
    vectorModel = glm::rotate(vectorModel, glm::radians(vectorRotation[1]), glm::vec3(0.0f, 1.0f, 0.0f));
    vectorModel = glm::rotate(vectorModel, glm::radians(vectorRotation[2]), glm::vec3(0.0f, 0.0f, 1.0f));
    vectorModel = glm::scale(vectorModel, glm::vec3(vectorScale[0], vectorScale[1], vectorScale[2]));
    wgpuQueueWriteBuffer(mQueue, mUniformVectorBuffer, offsetof(MyUniforms, modelMatrix), &vectorModel, sizeof(glm::mat4));


    ImGuiIO& io = ImGui::GetIO();
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
    ImGui::End();

    // Draw the UI
    ImGui::EndFrame();
    // Convert the UI defined above into low-level drawing commands
    ImGui::Render();
    // Execute the low-level drawing commands on the WebGPU backend
    ImGui_ImplWGPU_RenderDrawData(ImGui::GetDrawData(), renderPass);
}


////////////////////////////////////////////////////////////////////////////
// Utility Methods /////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

WGPUTextureView Renderer::getNextSurfaceTextureView()
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


void Renderer::processInput()
{
    float speed = 5.5f * deltaTime;

    Application& app = Application::get();
    GLFWwindow* window = static_cast<GLFWwindow*>(app.getWindow().getNativeWindow());

    // Camera Movement
	if (glfwGetKey(window, GLFW_KEY_W) && GLFW_PRESS)
	{
		mMainCamera.mPosition += speed * mMainCamera.mFront;
	}
	if (glfwGetKey(window, GLFW_KEY_S) && GLFW_PRESS)
	{
		mMainCamera.mPosition -= speed * mMainCamera.mFront;
	}
	if (glfwGetKey(window, GLFW_KEY_A) && GLFW_PRESS)
	{
		mMainCamera.mPosition -= speed * glm::normalize(glm::cross(mMainCamera.mFront, mMainCamera.mUp));
	}
	if (glfwGetKey(window, GLFW_KEY_D) && GLFW_PRESS)
	{
		mMainCamera.mPosition += speed * glm::normalize(glm::cross(mMainCamera.mFront, mMainCamera.mUp));
	}
	if (glfwGetKey(window,GLFW_KEY_SPACE) && GLFW_PRESS)
	{
		mMainCamera.mPosition += speed * mMainCamera.mUp;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) && GLFW_PRESS)
	{
		mMainCamera.mPosition -= speed * mMainCamera.mUp;
	}
}

void Renderer::processMouseMovement(double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);
    if(rightbuttonPressed)
    {

        if (firstMouse)
        {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
        }

        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
        lastX = xpos;
        lastY = ypos;

        float sensitivity = 0.1f; // change this value to your liking
        xoffset *= sensitivity;
        yoffset *= sensitivity;

        yaw += xoffset;
        pitch += yoffset;

        // make sure that when pitch is out of bounds, screen doesn't get flipped
        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;

        glm::vec3 front;
        front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        front.y = sin(glm::radians(pitch));
        front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        mMainCamera.mFront = glm::normalize(front);

    }
}

void Renderer::processMouseButtonCallback(int button, int action, int mods)
{
    Application& app = Application::get();
    GLFWwindow* window = static_cast<GLFWwindow*>(app.getWindow().getNativeWindow());

    if (button == GLFW_MOUSE_BUTTON_RIGHT) 
    {
        if (action == GLFW_PRESS) 
        {
            rightbuttonPressed = true;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);  // Esconde o cursor
        }
        if (action == GLFW_RELEASE) 
        {
            rightbuttonPressed = false;
            firstMouse = true;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);  // Mostra o cursor
        }
    }
}
