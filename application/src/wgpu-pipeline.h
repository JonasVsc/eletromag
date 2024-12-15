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