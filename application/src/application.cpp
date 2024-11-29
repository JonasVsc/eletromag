#include <emscripten.h>
#include <webgpu/webgpu.h>

#include <iostream>


int main(int argc, char** argv) {

    // We create a descriptor
    WGPUInstanceDescriptor desc = {};
    desc.nextInChain = nullptr;

    // We create the instance using this descriptor
    WGPUInstance instance = wgpuCreateInstance(nullptr);

    // We can check whether there is actually an instance created
    if (!instance) {
        std::cerr << "Could not initialize WebGPU!" << std::endl;
        return 1;
    }


    // Display the object (WGPUInstance is a simple pointer, it may be
    // copied around without worrying about its size).
    std::cout << "WGPU instance: " << instance << std::endl;


    return 0;
}