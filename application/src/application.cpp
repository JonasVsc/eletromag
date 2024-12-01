#include "application.h"

#include "webgpu-utils.h"
#include "glfw3webgpu.h"

#include<iostream>

const char* shaderSource = R"(
@vertex
fn vs_main(@builtin(vertex_index) in_vertex_index: u32) -> @builtin(position) vec4f {
	var p = vec2f(0.0, 0.0);
	if (in_vertex_index == 0u) {
		p = vec2f(-0.5, -0.5);
	} else if (in_vertex_index == 1u) {
		p = vec2f(0.5, -0.5);
	} else {
		p = vec2f(0.0, 0.5);
	}
	return vec4f(p, 0.0, 1.0);
}

@fragment
fn fs_main() -> @location(0) vec4f {
	return vec4f(0.0, 0.4, 1.0, 1.0);
}
)";

void Application::initialize()
{
	initWindow();
	initWebGPU();
}

void Application::terminate()
{
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
	renderPassColorAttachment.clearValue = WGPUColor{ 0.9, 0.1, 0.2, 1.0 };
	renderPassColorAttachment.depthSlice = WGPU_DEPTH_SLICE_UNDEFINED;

	renderPassDescriptor.colorAttachmentCount = 1;
	renderPassDescriptor.colorAttachments = &renderPassColorAttachment;
	renderPassDescriptor.depthStencilAttachment = nullptr;
	renderPassDescriptor.timestampWrites = nullptr;

	WGPURenderPassEncoder renderPass = wgpuCommandEncoderBeginRenderPass(commandEncoder, &renderPassDescriptor);

	wgpuRenderPassEncoderSetPipeline(renderPass, mPipeline);
	wgpuRenderPassEncoderDraw(renderPass, 3, 1, 0, 0);

	wgpuRenderPassEncoderEnd(renderPass);
	wgpuRenderPassEncoderRelease(renderPass);

	WGPUCommandBufferDescriptor commandBufferDescriptor{};
	commandBufferDescriptor.nextInChain = nullptr;
	commandBufferDescriptor.label = "Command buffer";
	WGPUCommandBuffer commandBuffer = wgpuCommandEncoderFinish(commandEncoder, &commandBufferDescriptor);
	wgpuCommandEncoderRelease(commandEncoder);

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

	initializeRenderPipeline();

}

void Application::initializeRenderPipeline()
{
	// load Shader Module
	WGPUShaderModuleDescriptor shaderDescriptor{};
	WGPUShaderModuleWGSLDescriptor shaderCodeDescriptor{};
	shaderCodeDescriptor.chain.next = nullptr;
	shaderCodeDescriptor.chain.sType = WGPUSType_ShaderModuleWGSLDescriptor;
	shaderDescriptor.nextInChain = &shaderCodeDescriptor.chain;
	shaderCodeDescriptor.code = shaderSource;
	WGPUShaderModule shaderModule = wgpuDeviceCreateShaderModule(mDevice, &shaderDescriptor);

	// create render pipeline
	WGPURenderPipelineDescriptor pipelineDescriptor{};
	pipelineDescriptor.nextInChain = nullptr;

	// vertex
	pipelineDescriptor.vertex.bufferCount = 0;
	pipelineDescriptor.vertex.buffers = nullptr;
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
	
	pipelineDescriptor.depthStencil = nullptr;
	pipelineDescriptor.multisample.count = 1;
	pipelineDescriptor.multisample.mask = ~0u;
	pipelineDescriptor.multisample.alphaToCoverageEnabled = false;
	pipelineDescriptor.layout = nullptr;

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
