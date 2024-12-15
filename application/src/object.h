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

    void update();

    void setPosition(float x, float y, float z);

    void setColor(float r, float g, float b, float a);

    virtual ~Object() = default;

    inline std::string getDebugName() { return mDebugName; }
    inline WGPURenderPipeline getRenderPipeline() { return mRenderPipeline; }
    inline WGPUBindGroup getBindGroup() { return mBindGroup; }
    inline WGPUBuffer getVertexBuffer() { return mVertexBuffer; }
    inline WGPUBuffer getUniformBuffer() { return mUniformBuffer; }
    inline MyUniforms& getUniform() { return mUniform; }
    inline uint32_t getVertexCount() { return mVertexCount; }

    float mPosition[3] {0.0f, 0.0f, 0.0f};
    float mRotation[3] {0.0f, 0.0f, 0.0f};
    float mScale[3] {1.0f, 1.0f, 1.0f};

    float mColor[4] {0.5f, 0.5f, 0.5f, 1.0f};

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