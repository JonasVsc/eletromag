#include <emscripten.h>
#include <webgpu/webgpu.h>

#include <iostream>
#include <cassert>

#include <vector>

WGPUAdapter requestAdapterSync(WGPUInstance instance, WGPURequestAdapterOptions const* options);


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

    // limits
    WGPUSupportedLimits supportedLimits{};
    bool success = wgpuAdapterGetLimits(adapter, &supportedLimits);

    if(success) {
        std::cout << "Adapter limits:" << '\n';
        std::cout << " - maxTextureDimension1D: " << supportedLimits.limits.maxTextureDimension1D << '\n';
        std::cout << " - maxTextureDimension2D: " << supportedLimits.limits.maxTextureDimension2D << '\n';
        std::cout << " - maxTextureDimension3D: " << supportedLimits.limits.maxTextureDimension3D << '\n';
        std::cout << " - maxTextureArrayLayers: " << supportedLimits.limits.maxTextureArrayLayers << '\n';
    }

    // features
    size_t featureCount = wgpuAdapterEnumerateFeatures(adapter, nullptr);
    std::vector<WGPUFeatureName> features(featureCount);
    wgpuAdapterEnumerateFeatures(adapter, features.data());

    std::cout << "Adapter features:" << '\n';
    std::cout << std::hex;
    for (const auto& f : features)
        std::cout << " - 0x" << f << '\n';

    // properties
    WGPUAdapterProperties properties{};
    properties.nextInChain = nullptr;
    wgpuAdapterGetProperties(adapter, &properties);

    std::cout << "Adapter properties:" << std::endl;
    std::cout << " - vendorID: " << properties.vendorID << std::endl;
    if (properties.vendorName) {
        std::cout << " - vendorName: " << properties.vendorName << std::endl;
    }
    if (properties.architecture) {
        std::cout << " - architecture: " << properties.architecture << std::endl;
    }
    std::cout << " - deviceID: " << properties.deviceID << std::endl;
    if (properties.name) {
        std::cout << " - name: " << properties.name << std::endl;
    }
    if (properties.driverDescription) {
        std::cout << " - driverDescription: " << properties.driverDescription << std::endl;
    }
    std::cout << std::hex;
    std::cout << " - adapterType: 0x" << properties.adapterType << std::endl;
    std::cout << " - backendType: 0x" << properties.backendType << std::endl;
    std::cout << std::dec; // Restore decimal numbers



    // destructors
    wgpuInstanceRelease(instance);
    wgpuAdapterRelease(adapter);
    return 0;
}

WGPUAdapter requestAdapterSync(WGPUInstance instance, WGPURequestAdapterOptions const* options)
{
    struct UserData
    {
        WGPUAdapter adapter = nullptr;
        bool requestEnded = false;
    };
    UserData userData;


    auto onAdapterRequestEnded = [](WGPURequestAdapterStatus status, WGPUAdapter adapter, char const* message, void* pUserData)
    {
        UserData& userData = *reinterpret_cast<UserData*>(pUserData);
        if (status == WGPURequestAdapterStatus_Success)
        {
            userData.adapter = adapter;
        }
        else
        {
            std::cout << "Could not get WebGPU adapter: " << message << std::endl;
        }
        userData.requestEnded = true;
    };

    wgpuInstanceRequestAdapter(instance, options, onAdapterRequestEnded, &userData);

    while (!userData.requestEnded)
    {
        emscripten_sleep(100);
    }

    assert(userData.requestEnded);

    return userData.adapter;
}