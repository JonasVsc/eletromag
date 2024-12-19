#include "webgpu-utils.h"

#include <iostream>
#include <cassert>

#include <vector>



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
                std::cout << "[ERROR] Could not get WebGPU adapter: " << message << std::endl;
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



WGPUDevice requestDeviceSync(WGPUAdapter adapter, WGPUDeviceDescriptor const* descriptor)
{
    struct UserData
    {
        WGPUDevice device = nullptr;
        bool requestEnded = false;
    };
    UserData userData;

    auto onDeviceRequestEnded = [](WGPURequestDeviceStatus status, WGPUDevice device, char const* message, void* pUserData)
        {
            UserData& userData = *reinterpret_cast<UserData*>(pUserData);
            if (status == WGPURequestDeviceStatus_Success)
            {
                userData.device = device;
            }
            else
            {
                std::cout << "[ERROR] Could not get WebGPU device: " << message << std::endl;
            }
            userData.requestEnded = true;

        };

    wgpuAdapterRequestDevice(adapter, descriptor, onDeviceRequestEnded, &userData);

    while (!userData.requestEnded)
    {
        emscripten_sleep(100);
    }

    assert(userData.requestEnded);

    return userData.device;
}

void inspectDevice(WGPUDevice device)
{
    size_t featureCount = wgpuDeviceEnumerateFeatures(device, nullptr);
    std::vector<WGPUFeatureName> features(featureCount);
    wgpuDeviceEnumerateFeatures(device, features.data());

    std::cout << "[INFO] Device features: " << '\n';
    std::cout << std::hex;
    for (const auto& f : features)
    {
        std::cout << " - 0x" << f << std::endl;
    }
    std::cout << std::dec;

    WGPUSupportedLimits limits{};
    limits.nextInChain = nullptr;

    bool success = wgpuDeviceGetLimits(device, &limits);

    if (success)
    {
        std::cout << "[INFO] Device limits:" << std::endl;
        std::cout << " - maxTextureDimension1D: " << limits.limits.maxTextureDimension1D << std::endl;
        std::cout << " - maxTextureDimension2D: " << limits.limits.maxTextureDimension2D << std::endl;
        std::cout << " - maxTextureDimension3D: " << limits.limits.maxTextureDimension3D << std::endl;
        std::cout << " - maxTextureArrayLayers: " << limits.limits.maxTextureArrayLayers << std::endl;
        // [...] Extra device limits
    }
}

void inspectAdapter(WGPUAdapter adapter)
{
    // limits
    WGPUSupportedLimits supportedLimits{};
    bool success = wgpuAdapterGetLimits(adapter, &supportedLimits);

    if (success) {
        std::cout << "[INFO] Adapter limits:" << '\n';
        std::cout << " - maxTextureDimension1D: " << supportedLimits.limits.maxTextureDimension1D << '\n';
        std::cout << " - maxTextureDimension2D: " << supportedLimits.limits.maxTextureDimension2D << '\n';
        std::cout << " - maxTextureDimension3D: " << supportedLimits.limits.maxTextureDimension3D << '\n';
        std::cout << " - maxTextureArrayLayers: " << supportedLimits.limits.maxTextureArrayLayers << '\n';
    }

    // features
    size_t featureCount = wgpuAdapterEnumerateFeatures(adapter, nullptr);
    std::vector<WGPUFeatureName> features(featureCount);
    wgpuAdapterEnumerateFeatures(adapter, features.data());

    std::cout << "[INFO] Adapter features:" << '\n';
    std::cout << std::hex;
    for (const auto& f : features)
        std::cout << " - 0x" << f << '\n';

    // properties
    WGPUAdapterProperties properties{};
    properties.nextInChain = nullptr;
    wgpuAdapterGetProperties(adapter, &properties);

    std::cout << "[INFO] Adapter properties:" << std::endl;
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
}
