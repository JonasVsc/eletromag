#pragma once

#include "definitions.h"
#include "physics.h"

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

    void reset();

    void update();

    virtual void physicsUpdate();

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

    float mVelocityDirection[3] {0.0f, 0.0f, 0.0f};
    float mVelocity = 0.0f;

    float mInitialPosition[3] {0.0f, 0.0f, 0.0f};
    float mInitialRotation[3] {0.0f, 0.0f, 0.0f};
    float mInitialScale[3] {1.0f, 1.0f, 1.0f};

    float mInitialColor[4] {0.5f, 0.5f, 0.5f, 1.0f};

    float mInitialVelocityDirection[3] {0.0f, 0.0f, 0.0f};
    float mInitialVelocity = 0.0f;

protected:

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