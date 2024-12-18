#include "object.h"

#include "application.h"
#include "wgpu-renderer2.h"
#include "utils/resource-manager.h"
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <utility>

#include "component.h"
#include "transform.h"



Object::Object(std::string debugName, const std::filesystem::path& path)
    : mDebugName(debugName)
{
    initRenderPipeline();
    initUniformData();
    initBuffers(path);
}

void Object::initRenderPipeline()
{
    Renderer2& renderer = Application::get().getRenderer();

    mShaderModule = ResourceManager::loadShaderModule("C:/Dev/eletromag/application/resources/shader.wgsl", renderer.getDevice());
    if (!mShaderModule) {
        std::cerr << "[ERROR] Failed to load shader module" << '\n';
    }

    WGPURenderPipelineDescriptor pipelineDesc{};

    std::vector<WGPUVertexAttribute> vertexAttribs(3);
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
    vertexBufferLayout.attributeCount = static_cast<uint32_t>(vertexAttribs.size());
    vertexBufferLayout.attributes = vertexAttribs.data();
    vertexBufferLayout.arrayStride = sizeof(ResourceManager::VertexAttributes);  
    vertexBufferLayout.stepMode = WGPUVertexStepMode_Vertex;

    pipelineDesc.vertex.bufferCount = 1;
    pipelineDesc.vertex.buffers = &vertexBufferLayout;
    pipelineDesc.vertex.module = mShaderModule;
    pipelineDesc.vertex.entryPoint = "vs_main";

    WGPUFragmentState fragmentState = {};
    fragmentState.module = mShaderModule;
    fragmentState.entryPoint = "fs_main"; 

    WGPUColorTargetState colorTarget = {};
    colorTarget.format = renderer.getSwapChainFormat();
    colorTarget.writeMask = WGPUColorWriteMask_All;
    fragmentState.targetCount = 1;
    fragmentState.targets = &colorTarget;

    pipelineDesc.fragment = &fragmentState;

    WGPUDepthStencilState depthStencilState{};
    depthStencilState.format = renderer.getDepthTextureFormat();
    depthStencilState.depthWriteEnabled = true; 
    depthStencilState.depthCompare = WGPUCompareFunction_Less;  
    depthStencilState.stencilReadMask = 0xFF;
    depthStencilState.stencilWriteMask = 0xFF;
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


    WGPUBindGroupLayoutEntry bindingLayout{};
    bindingLayout.binding = 0;
    bindingLayout.visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment;
    bindingLayout.buffer.type = WGPUBufferBindingType_Uniform;
    bindingLayout.buffer.minBindingSize = sizeof(MyUniforms);

    WGPUBindGroupLayoutDescriptor bindGroupLayoutDesc{};
    bindGroupLayoutDesc.entryCount = 1;
    bindGroupLayoutDesc.entries = &bindingLayout;
    mBindGroupLayout = wgpuDeviceCreateBindGroupLayout(renderer.getDevice(), &bindGroupLayoutDesc);


    WGPUPipelineLayoutDescriptor layoutDesc{};
    layoutDesc.bindGroupLayoutCount = 1;
    layoutDesc.bindGroupLayouts = &mBindGroupLayout;
    WGPUPipelineLayout layout = wgpuDeviceCreatePipelineLayout(renderer.getDevice(), &layoutDesc);
    pipelineDesc.layout = layout;

    mRenderPipeline = wgpuDeviceCreateRenderPipeline(renderer.getDevice(), &pipelineDesc);
    std::cout << "[INFO] Render pipeline created successfully" << '\n';

    if (!mRenderPipeline) {
        std::cerr << "[ERROR] Failed to create render pipeline" << '\n';
    }
}

void Object::initUniformData()
{
    Camera& camera = Application::get().getMainCamera();

    mUniform.modelMatrix = glm::mat4x4(1.0f);
    mUniform.viewMatrix = camera.getViewMatrix();
    mUniform.projectionMatrix = glm::perspective(glm::radians(camera.fov), camera.aspectRatio, camera.nearPlane, camera.farPlane);
}


void Object::initBuffers(const std::filesystem::path& path)
{
    Renderer2& renderer = Application::get().getRenderer();

    std::vector<ResourceManager::VertexAttributes> vertexData;
    bool success = ResourceManager::loadGeometryFromObj(path, vertexData);
    if(!success)
        std::cerr << "[ERROR] Failed load geometry" << '\n';
    mVertexCount = static_cast<uint32_t>(vertexData.size());

    WGPUBufferDescriptor bufferDesc{};
    // vertex buffer
    // -------------
    bufferDesc.size = vertexData.size() * sizeof(ResourceManager::VertexAttributes);
    bufferDesc.usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Vertex;
    bufferDesc.mappedAtCreation = false;
    mVertexBuffer = wgpuDeviceCreateBuffer(renderer.getDevice(), &bufferDesc);
    wgpuQueueWriteBuffer(renderer.getQueue(), mVertexBuffer, 0, vertexData.data(), bufferDesc.size);

    // uniform buffer
    // --------------
    bufferDesc.size = sizeof(MyUniforms);
    bufferDesc.usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform;
    bufferDesc.mappedAtCreation = false;
    mUniformBuffer = wgpuDeviceCreateBuffer(renderer.getDevice(), &bufferDesc);
    wgpuQueueWriteBuffer(renderer.getQueue(), mUniformBuffer, 0, &mUniform, sizeof(MyUniforms));
    if(!mUniformBuffer)
        std::cerr << "[ERROR] Failed to init uniform buffer" << '\n';

    // bind group
    // ----------
    WGPUBindGroupEntry bindings{};
    bindings.binding = 0;
    bindings.buffer = mUniformBuffer;
    bindings.offset = 0;
    bindings.size = sizeof(MyUniforms);

    WGPUBindGroupDescriptor bindGroupDesc{};
    bindGroupDesc.layout = mBindGroupLayout;
    bindGroupDesc.entryCount = 1;
    bindGroupDesc.entries = &bindings;
    mBindGroup = wgpuDeviceCreateBindGroup(renderer.getDevice(), &bindGroupDesc);
    if(!mBindGroup)
        std::cerr << "[ERROR] Failed to init bind group" << '\n';
}

void Object::update()
{
    if(!Application::sRunningSimulation)
    {
        reset();
        return;
    }


}

void Object::reset()
{
    // transform.mPosition = transform.mInitialPosition;
    // transform.mPosition = transform.mInitialPosition;
    // transform.mPosition = transform.mInitialPosition;
    // transform.mRotation = transform.mInitialRotation;
    // transform.mRotation = transform.mInitialRotation;
    // transform.mRotation = transform.mInitialRotation;
    // transform.mScale = transform.mInitialScale;
    // transform.mScale = transform.mInitialScale;
    // transform.mScale = transform.mInitialScale;

}

void Object::render()
{
    Renderer2& renderer = Application::get().getRenderer();
    Camera& camera = Application::get().getMainCamera();

    // update viewMatrix
    glm::mat4 view(1.0f);
    view = camera.getViewMatrix();
    wgpuQueueWriteBuffer(renderer.getQueue(), getUniformBuffer(), offsetof(MyUniforms, viewMatrix), &view, sizeof(glm::mat4));

    for (auto& comp : components)
        comp->update(Application::deltaTime);
}

