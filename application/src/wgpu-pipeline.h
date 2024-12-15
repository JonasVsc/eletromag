#pragma once

#include <emscripten.h>
#include <webgpu/webgpu.h>
#include <vector>

class Pipeline
{
public:

    ~Pipeline();
    
    void init();

    void render();

    void terminate();

    inline WGPURenderPipeline getRenderPipeline() { return mRenderPipeline; }

    inline WGPUBindGroupLayout getBindGroupLayout() { return mBindGroupLayout; }

private:

    void initAndLoadShaders();

    std::vector<WGPUVertexAttribute> initVertexAttribs();

    WGPUVertexBufferLayout initVertexBufferLayout(const std::vector<WGPUVertexAttribute> &vertexAttribs);

    WGPUFragmentState initFragmentState();

    WGPUDepthStencilState initDepthStencilState();

    WGPUPipelineLayout initPipelineLayout();


private:
    WGPURenderPipeline mRenderPipeline;

    WGPUShaderModule mShaderModule;

    WGPUBindGroupLayout mBindGroupLayout;
};