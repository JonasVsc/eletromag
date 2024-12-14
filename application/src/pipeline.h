#pragma once

#include <emscripten.h>
#include <webgpu/webgpu.h>

class Pipeline
{
public:

    Pipeline(WGPUDevice device);
    
    ~Pipeline();
    
    void createPipeline();

    void render();

private:

    WGPUDevice device;
    WGPURenderPipeline pipeline;
};