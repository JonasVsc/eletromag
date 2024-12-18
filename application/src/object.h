#pragma once



#include "definitions.h"
#include "transform.h"
#include "physics.h"

#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <webgpu/webgpu.h>
#include <filesystem>
#include <iostream>


class Object
{
public:

    enum class Type { Field, Eletron, Arrow, None };

    virtual Type getType() const { return Type::None; }

    Object(std::string debugName, const std::filesystem::path& path);

    void initRenderPipeline();

    void initUniformData();

    void initBuffers(const std::filesystem::path& path);

    void reset();

    void render();

    virtual void update();

    virtual ~Object() = default;

    inline std::string getDebugName() { return mDebugName; }
    inline WGPURenderPipeline getRenderPipeline() { return mRenderPipeline; }
    inline WGPUBindGroup getBindGroup() { return mBindGroup; }
    inline WGPUBuffer getVertexBuffer() { return mVertexBuffer; }
    inline WGPUBuffer getUniformBuffer() { return mUniformBuffer; }
    inline MyUniforms& getUniform() { return mUniform; }
    inline uint32_t getVertexCount() { return mVertexCount; }



    Transform transform;

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