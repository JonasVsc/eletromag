#include "application.h"

#include "webgpu-utils.h"
#include "glfw3webgpu.h"
#include "resource-manager.h"

#include<iostream>
#include<cstdint>
#include<stdint.h>
#include<vector>

void Application::initialize()
{
	initWindow();
	initWebGPU();
}

void Application::terminate()
{
	wgpuTextureViewRelease(mDepthTextureView);
	wgpuBindGroupRelease(mBindGroup);
	wgpuPipelineLayoutRelease(mLayout);
	wgpuBindGroupLayoutRelease(mBindGroupLayout);
	wgpuBufferRelease(uniformBuffer);
	wgpuBufferRelease(indexBuffer);
	wgpuBufferRelease(vertexBuffer);
	wgpuRenderPipelineRelease(mPipeline);
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

	float time = static_cast<float>(glfwGetTime());
	wgpuQueueWriteBuffer(mQueue, uniformBuffer, offsetof(MyUniforms, time), &time, sizeof(float));

	WGPUTextureView targetView = getNextSurfaceTextureView();
	if (!targetView) return;

	// command encoder
	WGPUCommandEncoderDescriptor encoderDescriptor = {};
	encoderDescriptor.nextInChain = nullptr;
	encoderDescriptor.label = "My command encoder";
	WGPUCommandEncoder commandEncoder = wgpuDeviceCreateCommandEncoder(mDevice, &encoderDescriptor);

	WGPURenderPassDescriptor renderPassDescriptor = {};
	renderPassDescriptor.nextInChain = nullptr;

	WGPURenderPassColorAttachment renderPassColorAttachment = {};
	renderPassColorAttachment.view = targetView;
	renderPassColorAttachment.resolveTarget = nullptr;
	renderPassColorAttachment.loadOp = WGPULoadOp_Clear;
	renderPassColorAttachment.storeOp = WGPUStoreOp_Store;
	renderPassColorAttachment.clearValue = WGPUColor{ 0.5, 1.0, 1.0, 1.0 };
	renderPassColorAttachment.depthSlice = WGPU_DEPTH_SLICE_UNDEFINED;

	renderPassDescriptor.colorAttachmentCount = 1;
	renderPassDescriptor.colorAttachments = &renderPassColorAttachment;

	WGPURenderPassDepthStencilAttachment depthStencilAttachment;
	depthStencilAttachment.view = mDepthTextureView;
	depthStencilAttachment.depthClearValue = 1.0f;
	depthStencilAttachment.depthLoadOp = WGPULoadOp_Clear;
	depthStencilAttachment.depthStoreOp = WGPUStoreOp_Store;
	depthStencilAttachment.depthReadOnly = false;
	depthStencilAttachment.stencilClearValue = 0;
	depthStencilAttachment.stencilLoadOp = WGPULoadOp_Undefined;
	depthStencilAttachment.stencilStoreOp = WGPUStoreOp_Undefined;
	depthStencilAttachment.stencilReadOnly = true;


	renderPassDescriptor.depthStencilAttachment = &depthStencilAttachment;
	renderPassDescriptor.timestampWrites = nullptr;

	WGPURenderPassEncoder renderPass = wgpuCommandEncoderBeginRenderPass(commandEncoder, &renderPassDescriptor);

	wgpuRenderPassEncoderSetPipeline(renderPass, mPipeline);
	wgpuRenderPassEncoderSetVertexBuffer(renderPass, 0, vertexBuffer, 0, wgpuBufferGetSize(vertexBuffer));
	wgpuRenderPassEncoderSetIndexBuffer(renderPass, indexBuffer, WGPUIndexFormat_Uint16, 0, wgpuBufferGetSize(indexBuffer));

	wgpuRenderPassEncoderSetBindGroup(renderPass, 0, mBindGroup, 0, nullptr);

	wgpuRenderPassEncoderDrawIndexed(renderPass, indexCount, 1, 0, 0, 0);

	wgpuRenderPassEncoderEnd(renderPass);
	wgpuRenderPassEncoderRelease(renderPass);

	WGPUCommandBufferDescriptor commandBufferDescriptor{};
	commandBufferDescriptor.nextInChain = nullptr;
	commandBufferDescriptor.label = "Command buffer";
	WGPUCommandBuffer commandBuffer = wgpuCommandEncoderFinish(commandEncoder, &commandBufferDescriptor);
	wgpuCommandEncoderRelease(commandEncoder);

	wgpuQueueSubmit(mQueue, 1, &commandBuffer);
	wgpuCommandBufferRelease(commandBuffer);

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

void wgpuPollEvents(bool yieldToWebBrowser)
{
	if(yieldToWebBrowser)
	{
		emscripten_sleep(100);
	}
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

	deviceCapabilities(adapter);
	
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

	initializeRenderPipeline();

	initializeBuffers();

	initializeBindGroups();
	
}

void Application::playingWithBuffers()
{
	// Experimentation for the "Playing with buffer" chapter
	WGPUBufferDescriptor bufferDesc = {};
	bufferDesc.nextInChain = nullptr;
	bufferDesc.label = "Some GPU-side data buffer";
	bufferDesc.usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_CopySrc;
	bufferDesc.size = 16;
	bufferDesc.mappedAtCreation = false;
	WGPUBuffer buffer1 = wgpuDeviceCreateBuffer(mDevice, &bufferDesc);
	bufferDesc.label = "Output buffer";
	bufferDesc.usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_MapRead;
	WGPUBuffer buffer2 = wgpuDeviceCreateBuffer(mDevice, &bufferDesc);
	
	// Create some CPU-side data buffer (of size 16 bytes)
	std::vector<uint8_t> numbers(16);
	for (uint8_t i = 0; i < 16; ++i) numbers[i] = i;
	// `numbers` now contains [ 0, 1, 2, ... ]
	
	// Copy this from `numbers` (RAM) to `buffer1` (VRAM)
	wgpuQueueWriteBuffer(mQueue, buffer1, 0, numbers.data(), numbers.size());
	
	WGPUCommandEncoder encoder = wgpuDeviceCreateCommandEncoder(mDevice, nullptr);
	
	// After creating the command encoder
	wgpuCommandEncoderCopyBufferToBuffer(encoder, buffer1, 0, buffer2, 0, 16);
	
	WGPUCommandBuffer command = wgpuCommandEncoderFinish(encoder, nullptr);
	wgpuCommandEncoderRelease(encoder);
	wgpuQueueSubmit(mQueue, 1, &command);
	wgpuCommandBufferRelease(command);
	
	// The context shared between this main function and the callback.
	struct Context {
		bool ready;
		WGPUBuffer buffer;
	};
	
	auto onBuffer2Mapped = [](WGPUBufferMapAsyncStatus status, void* pUserData) {
		Context* context = reinterpret_cast<Context*>(pUserData);
		context->ready = true;
		std::cout << "Buffer 2 mapped with status " << status << std::endl;
		if (status != WGPUBufferMapAsyncStatus_Success) return;
	
		// Get a pointer to wherever the driver mapped the GPU memory to the RAM
		uint8_t* bufferData = (uint8_t*)wgpuBufferGetConstMappedRange(context->buffer, 0, 16);
		
		std::cout << "bufferData = [";
		for (int i = 0; i < 16; ++i) {
			if (i > 0) std::cout << ", ";
			std::cout << (int)bufferData[i];
		}
		std::cout << "]" << std::endl;
		
		// Then do not forget to unmap the memory
		wgpuBufferUnmap(context->buffer);
	};
	
	// Create the Context instance
	Context context = { false, buffer2 };
	
	wgpuBufferMapAsync(buffer2, WGPUMapMode_Read, 0, 16, onBuffer2Mapped, (void*)&context);
	//                      Pass the address of the Context instance here: ^^^^^^^^^^^^^^
	
	while (!context.ready) {
		//  ^^^^^^^^^^^^^ Use context.ready here instead of ready
		wgpuPollEvents(true /* yieldToBrowser */);
	}
	
	// In Terminate()
	wgpuBufferRelease(buffer1);
	wgpuBufferRelease(buffer2);
}

void Application::initializeBuffers()
{
	std::vector<float> vertexData;
	std::vector<uint16_t> indexData;

	bool success = ResourceManager::loadGeometry("C:/Dev/eletromag/application/resources/pyramid.txt", vertexData, indexData, 3);

	if(!success)
		std::cerr << "[ERROR] could not load geometry" << '\n';
	
	vertexCount = static_cast<uint32_t>(vertexData.size() / 4);
	indexCount = static_cast<uint32_t>(indexData.size());

	// vertex buffer
	WGPUBufferDescriptor bufferDescriptor{};
	bufferDescriptor.label = "vertex buffer";
	bufferDescriptor.nextInChain = nullptr;
	bufferDescriptor.size = vertexData.size() * sizeof(float);
	bufferDescriptor.usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Vertex;
	bufferDescriptor.mappedAtCreation = false;
	
	vertexBuffer = wgpuDeviceCreateBuffer(mDevice, &bufferDescriptor);
	wgpuQueueWriteBuffer(mQueue, vertexBuffer, 0, vertexData.data(), bufferDescriptor.size);

	// index buffer
	bufferDescriptor.size = indexData.size() * sizeof(uint16_t);
	bufferDescriptor.size = (bufferDescriptor.size + 3) & ~3; // round up to the next multiple of 4
	bufferDescriptor.usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Index;

	indexBuffer = wgpuDeviceCreateBuffer(mDevice, &bufferDescriptor);
	wgpuQueueWriteBuffer(mQueue, indexBuffer, 0, indexData.data(), bufferDescriptor.size);

	// uniform buffer
	bufferDescriptor.size = sizeof(MyUniforms);
	bufferDescriptor.usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform;
	bufferDescriptor.mappedAtCreation = false;
	uniformBuffer = wgpuDeviceCreateBuffer(mDevice, &bufferDescriptor);

	MyUniforms uniforms;
	uniforms.time = 1.0f;
	uniforms.color = { 0.0f, 1.0f, 0.4f, 1.0f };
	wgpuQueueWriteBuffer(mQueue, uniformBuffer, 0, &uniforms, sizeof(MyUniforms));
}

void Application::initializeBindGroups()
{
	// binding
	WGPUBindGroupEntry binding{};
	binding.nextInChain = nullptr;
	binding.binding = 0;
	binding.buffer = uniformBuffer;
	binding.offset = 0;
	binding.size = sizeof(MyUniforms);

	WGPUBindGroupDescriptor bindGroupDescriptor{};
	bindGroupDescriptor.nextInChain = nullptr;
	bindGroupDescriptor.layout = mBindGroupLayout;
	// There must be as many bindings as declared in the layout!
	bindGroupDescriptor.entryCount = 1;
	bindGroupDescriptor.entries = &binding;
	mBindGroup = wgpuDeviceCreateBindGroup(mDevice, &bindGroupDescriptor);
}

void Application::deviceCapabilities(WGPUAdapter adapter)
{
	WGPUSupportedLimits supportedLimits{};
	supportedLimits.nextInChain = nullptr;

	wgpuAdapterGetLimits(adapter, &supportedLimits);
	std::cout << "adapter.maxVertexAttributes: " << supportedLimits.limits.maxVertexAttributes << std::endl;

	wgpuDeviceGetLimits(mDevice, &supportedLimits);
	std::cout << "device.maxVertexAttributes: " << supportedLimits.limits.maxVertexAttributes << std::endl;

}



void Application::initializeRenderPipeline()
{
	
	WGPUShaderModule shaderModule = ResourceManager::loadShaderModule("C:/Dev/eletromag/application/resources/shader.wgsl", mDevice);

	// create render pipeline
	WGPURenderPipelineDescriptor pipelineDescriptor{};
	pipelineDescriptor.nextInChain = nullptr;

	WGPUVertexBufferLayout bufferLayout{};
	
	std::vector<WGPUVertexAttribute> vertexAttributes(2);

	// position attribute
	vertexAttributes[0].shaderLocation = 0;
	vertexAttributes[0].format = WGPUVertexFormat_Float32x3;
	vertexAttributes[0].offset = 0;

	vertexAttributes[1].shaderLocation = 1;
	vertexAttributes[1].format = WGPUVertexFormat_Float32x3;
	vertexAttributes[1].offset = 3 * sizeof(float);

	bufferLayout.attributeCount = static_cast<uint32_t>(vertexAttributes.size());
	bufferLayout.attributes = vertexAttributes.data();
	bufferLayout.arrayStride = 6 * sizeof(float);
	bufferLayout.stepMode = WGPUVertexStepMode_Vertex;

	// vertex
	pipelineDescriptor.vertex.bufferCount = 1;
	pipelineDescriptor.vertex.buffers = &bufferLayout;
	pipelineDescriptor.vertex.module = shaderModule;
	pipelineDescriptor.vertex.entryPoint = "vs_main";
	pipelineDescriptor.vertex.constantCount = 0;
	pipelineDescriptor.vertex.constants = nullptr;

	// primitive
	pipelineDescriptor.primitive.topology = WGPUPrimitiveTopology_TriangleList;
	pipelineDescriptor.primitive.stripIndexFormat = WGPUIndexFormat_Undefined;
	pipelineDescriptor.primitive.frontFace = WGPUFrontFace_CCW;
	pipelineDescriptor.primitive.cullMode = WGPUCullMode_None;

	// fragment
	WGPUFragmentState fragmentState{};
	fragmentState.module = shaderModule;
	fragmentState.entryPoint = "fs_main";
	fragmentState.constantCount = 0;
	fragmentState.constants = nullptr;

	//blend
	WGPUBlendState blendState{};
	blendState.color.srcFactor = WGPUBlendFactor_SrcAlpha;
	blendState.color.dstFactor = WGPUBlendFactor_OneMinusSrcAlpha;
	blendState.color.operation = WGPUBlendOperation_Add;
	blendState.alpha.srcFactor = WGPUBlendFactor_Zero;
	blendState.alpha.dstFactor = WGPUBlendFactor_One;
	blendState.alpha.operation = WGPUBlendOperation_Add;

	WGPUColorTargetState colorTarget{};
	colorTarget.format = mSurfaceFormat;
	colorTarget.blend = &blendState;
	colorTarget.writeMask = WGPUColorWriteMask_All;
	
	fragmentState.targetCount = 1;
	fragmentState.targets = &colorTarget;
	pipelineDescriptor.fragment = &fragmentState;
	
	// depth/stencil
	WGPUDepthStencilState depthStencilState{};
    depthStencilState.depthWriteEnabled = false;
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
	

	

	WGPUTextureFormat depthTextureFormat = WGPUTextureFormat_Depth24Plus;
	depthStencilState.format = depthTextureFormat;
	
	// depth texture
	WGPUTextureDescriptor depthTextureDescriptor{};
	depthTextureDescriptor.dimension = WGPUTextureDimension_2D;
	depthTextureDescriptor.format = depthTextureFormat;
	depthTextureDescriptor.mipLevelCount = 1;
	depthTextureDescriptor.sampleCount = 1;
	depthTextureDescriptor.size = {640, 480, 1};
	depthTextureDescriptor.usage = WGPUTextureUsage_RenderAttachment;
	depthTextureDescriptor.viewFormatCount = 1;
	depthTextureDescriptor.viewFormats = &depthTextureFormat;
	WGPUTexture depthTexture = wgpuDeviceCreateTexture(mDevice, &depthTextureDescriptor);

	WGPUTextureViewDescriptor depthTextureViewDescriptor{};
	depthTextureViewDescriptor.aspect = WGPUTextureAspect_DepthOnly;
	depthTextureViewDescriptor.baseArrayLayer = 0;
	depthTextureViewDescriptor.arrayLayerCount = 1;
	depthTextureViewDescriptor.baseMipLevel = 0;
	depthTextureViewDescriptor.mipLevelCount = 1;
	depthTextureViewDescriptor.dimension = WGPUTextureViewDimension_2D;
	depthTextureViewDescriptor.format = depthTextureFormat;
	mDepthTextureView = wgpuTextureCreateView(depthTexture, &depthTextureViewDescriptor);
	

	pipelineDescriptor.depthStencil = &depthStencilState;



	pipelineDescriptor.multisample.count = 1;
	pipelineDescriptor.multisample.mask = ~0u;
	pipelineDescriptor.multisample.alphaToCoverageEnabled = false;

	

	WGPUBindGroupLayoutEntry bindingLayout{};
	bindingLayout.buffer.nextInChain = nullptr;
    bindingLayout.buffer.type = WGPUBufferBindingType_Undefined;
    bindingLayout.buffer.hasDynamicOffset = false;

    bindingLayout.sampler.nextInChain = nullptr;
    bindingLayout.sampler.type = WGPUSamplerBindingType_Undefined;

    bindingLayout.storageTexture.nextInChain = nullptr;
    bindingLayout.storageTexture.access = WGPUStorageTextureAccess_Undefined;
    bindingLayout.storageTexture.format = WGPUTextureFormat_Undefined;
    bindingLayout.storageTexture.viewDimension = WGPUTextureViewDimension_Undefined;

    bindingLayout.texture.nextInChain = nullptr;
    bindingLayout.texture.multisampled = false;
    bindingLayout.texture.sampleType = WGPUTextureSampleType_Undefined;
    bindingLayout.texture.viewDimension = WGPUTextureViewDimension_Undefined;
	bindingLayout.binding = 0;
	bindingLayout.visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment;
	bindingLayout.buffer.type = WGPUBufferBindingType_Uniform;
	bindingLayout.buffer.minBindingSize = sizeof(MyUniforms);

	// bind group layout
	WGPUBindGroupLayoutDescriptor bindGroupLayoutDescriptor{};
	bindGroupLayoutDescriptor.nextInChain = nullptr;
	bindGroupLayoutDescriptor.entryCount = 1;
	bindGroupLayoutDescriptor.entries = &bindingLayout;
	mBindGroupLayout = wgpuDeviceCreateBindGroupLayout(mDevice, &bindGroupLayoutDescriptor);

	// pipeline layout
	WGPUPipelineLayoutDescriptor pipelineLayoutDescriptor{};
	pipelineLayoutDescriptor.nextInChain = nullptr;
	pipelineLayoutDescriptor.bindGroupLayoutCount = 1;
	pipelineLayoutDescriptor.bindGroupLayouts = &mBindGroupLayout;
	mLayout = wgpuDeviceCreatePipelineLayout(mDevice, &pipelineLayoutDescriptor);

	pipelineDescriptor.layout = mLayout;

	mPipeline = wgpuDeviceCreateRenderPipeline(mDevice, &pipelineDescriptor);

	wgpuShaderModuleRelease(shaderModule);
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
