#pragma once

#include <emscripten.h>
#include <webgpu/webgpu.h>


class Renderer 
{
public:

    void init();

    void update();

    void terminate();

private:

    void initDevice();

private:

    WGPUInstance mInstance;

    WGPUSurface mSurface;
};