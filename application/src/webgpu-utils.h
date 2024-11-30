#pragma once

#include <emscripten.h>
#include <webgpu/webgpu.h>


WGPUAdapter requestAdapterSync(WGPUInstance instance, WGPURequestAdapterOptions const* options);

WGPUDevice requestDeviceSync(WGPUAdapter adapter, WGPUDeviceDescriptor const* descriptor);

void inspectDevice(WGPUDevice device);

void inspectAdapter(WGPUAdapter adapter);