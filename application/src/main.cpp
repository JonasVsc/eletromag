#include <emscripten.h>
#include <webgpu/webgpu.h>

#include "webgpu-utils.h"

#include <iostream>
#include <cassert>

#include <vector>




int main(int argc, char** argv) 
{
    WGPUInstance instance = wgpuCreateInstance(nullptr);

    if (!instance)
    {
        std::cerr << "Could not initialize WebGPU!" << std::endl;
        return 1;
    }
    
    std::cout << "WGPU instance: " << instance << std::endl;

    std::cout << "Requesting adapter..." << std::endl;

    WGPURequestAdapterOptions adapterOpts = {};
    adapterOpts.nextInChain = nullptr;
    WGPUAdapter adapter = requestAdapterSync(instance, &adapterOpts);
    std::cout << "Got adapter: " << adapter << std::endl;

    // display adapter info
    inspectAdapter(adapter);

    wgpuInstanceRelease(instance);

    WGPUDeviceDescriptor deviceDesc = {};
    deviceDesc.nextInChain = nullptr;
    deviceDesc.label = "My Device"; // anything works here, that's your call
    deviceDesc.requiredFeatureCount = 0; // we do not require any specific feature
    deviceDesc.requiredLimits = nullptr; // we do not require any specific limit
    deviceDesc.defaultQueue.nextInChain = nullptr;
    deviceDesc.defaultQueue.label = "The default queue";
    deviceDesc.deviceLostCallback = nullptr;

    std::cout << "Requesting device..." << std::endl;
    WGPUDevice device = requestDeviceSync(adapter, &deviceDesc);
    std::cout << "Got device: " << device << std::endl;

    // display device info
    inspectDevice(device);

    deviceDesc.deviceLostCallback = [](WGPUDeviceLostReason reason, char const* message, void* userdata)
    {
        std::cout << "Device lost: reason " << reason << '\n';
        if (message) std::cout << " (" << message << ')' << '\n';
    };

    auto onDeviceError = [](WGPUErrorType type, char const* message, void* /* pUserData */) {
        std::cout << "Uncaptured device error: type " << type;
        if (message) std::cout << " (" << message << ")";
        std::cout << std::endl;
        };
    wgpuDeviceSetUncapturedErrorCallback(device, onDeviceError, nullptr /* pUserData */);

    wgpuAdapterRelease(adapter);

    // GPU Queue

    WGPUQueue queue = wgpuDeviceGetQueue(device);

    auto onQueueWorkDone = [](WGPUQueueWorkDoneStatus status, void* userdata)
    {
        std::cout << "Queue work finished with status: " << status << '\n';
    };
    wgpuQueueOnSubmittedWorkDone(queue, onQueueWorkDone, nullptr);

    // command encoder

    WGPUCommandEncoderDescriptor encoderDesc{};
    encoderDesc.nextInChain = nullptr;
    encoderDesc.label = "My command encoder";
    WGPUCommandEncoder encoder = wgpuDeviceCreateCommandEncoder(device, &encoderDesc);

    wgpuCommandEncoderInsertDebugMarker(encoder, "Do one thing");
    wgpuCommandEncoderInsertDebugMarker(encoder, "Do another thing");

    WGPUCommandBufferDescriptor cmdBufferDescriptor{};
    cmdBufferDescriptor.nextInChain = nullptr;
    cmdBufferDescriptor.label = "Command buffer";
    WGPUCommandBuffer command = wgpuCommandEncoderFinish(encoder, &cmdBufferDescriptor);
    wgpuCommandEncoderRelease(encoder);

    std::cout << "Submiting command..." << std::endl;
    wgpuQueueSubmit(queue, 1, &command);
    wgpuCommandBufferRelease(command);
    std::cout << "Command submited." << std::endl;

    for (int i = 0; i < 5; ++i) 
    {
        std::cout << "Tick/Poll device..." << std::endl;
        emscripten_sleep(100);
    }

    wgpuDeviceRelease(device);
    wgpuQueueRelease(queue);
    return 0;
}

