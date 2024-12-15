#pragma once

#include "definitions.h"

#include <webgpu/webgpu.h>
#include <filesystem>
#include <iostream>

class Object
{
public:

    Object(std::string debugName, const std::filesystem::path& path);

    void initRenderPipeline();

    void initUniformData();

    void initBuffers(const std::filesystem::path& path);

    void setPosition(const glm::vec3& pos);

    void setColor(const glm::vec4& color);

    virtual ~Object() = default;

    inline std::string getDebugName() { return mDebugName; }
    inline WGPURenderPipeline getRenderPipeline() { return mRenderPipeline; }
    inline WGPUBindGroup getBindGroup() { return mBindGroup; }
    inline WGPUBuffer getVertexBuffer() { return mVertexBuffer; }
    inline WGPUBuffer getUniformBuffer() { return mUniformBuffer; }
    inline uint32_t getVertexCount() { return mVertexCount; }

private:

    WGPUShaderModule mShaderModule;

    WGPURenderPipeline mRenderPipeline;

    WGPUBindGroupLayout mBindGroupLayout;

    WGPUBindGroup mBindGroup;

    WGPUBuffer mVertexBuffer;

    uint32_t mVertexCount;

    WGPUBuffer mUniformBuffer;
    
    MyUniforms mUniform;

    std::string mDebugName;

};