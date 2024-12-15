#pragma once

#include <iostream>

#include <webgpu/webgpu.h>


class Layer
{

public:
    Layer(const std::string& debugName);

    virtual ~Layer();

    virtual void onAttach() {}

    virtual void onDettach() {}

    virtual void onUpdate(WGPURenderPassEncoder renderPass) {}

    inline std::string getDebugName() { return mDebugName; }

protected:

    std::string mDebugName;

};