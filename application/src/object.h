#pragma once

#include "application.h"
#include "resource-manager.h"

#include <webgpu/webgpu.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <iostream>

constexpr float PI = 3.14159265358979323846f;

class Object
{
public:

    Object()
    {
    };

    virtual void configure(const std::filesystem::path& path) 
    {
        Pipeline& pipeline = Application::get().getPipeline();
        Renderer2& renderer = Application::get().getRenderer();
        Camera& camera = Application::get().getMainCamera();

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
        mUniform.modelMatrix = glm::mat4x4(1.0f);
        mUniform.viewMatrix = camera.getViewMatrix();
        mUniform.projectionMatrix = glm::perspective(45 * PI / 180, 640.0f / 480.0f, 0.01f, 100.0f);
        mUniform.color = { 0.0f, 0.0f, 1.0f, 1.0f };
        mUniform.direction = { 0.0f, 0.0f, 0.0f};
        mUniform.intensity = 0.0f;
        mUniform.mass = 0.0f;
        
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
        bindGroupDesc.layout = pipeline.getBindGroupLayout();
        bindGroupDesc.entryCount = 1;
        bindGroupDesc.entries = &bindings;
        mBindGroup = wgpuDeviceCreateBindGroup(renderer.getDevice(), &bindGroupDesc);
        if(!mBindGroup)
            std::cerr << "[ERROR] Failed to init bind group" << '\n';
    }

    virtual void draw(WGPURenderPassEncoder renderPass)
    {
        update();


        wgpuRenderPassEncoderSetVertexBuffer(renderPass, 0, mVertexBuffer, 0, wgpuBufferGetSize(mVertexBuffer));
        wgpuRenderPassEncoderSetBindGroup(renderPass, 0, mBindGroup, 0, nullptr);
        wgpuRenderPassEncoderDraw(renderPass, mVertexCount, 1, 0, 0);
    }

    virtual void update() 
    {
        // update camera
        Camera& camera = Application::get().getMainCamera();
        Renderer2& renderer = Application::get().getRenderer();
        glm::mat4 view(1.0f);
        view = camera.getViewMatrix();

        wgpuQueueWriteBuffer(renderer.getQueue(), mUniformBuffer, offsetof(MyUniforms, viewMatrix), &view, sizeof(glm::mat4));

    }

    virtual ~Object() = default;

    WGPURenderPipeline mRenderPipeline;

    WGPUBuffer mVertexBuffer;

    uint32_t mVertexCount;

    WGPUBuffer mUniformBuffer;

    MyUniforms mUniform;

    WGPUBindGroup mBindGroup;

};