#pragma once
#include <vector>
#include <filesystem>
#include <webgpu/webgpu.h>

class ResourceManager
{
public:
    static bool loadGeometry(const std::filesystem::path& path, std::vector<float>& pointData, std::vector<uint16_t>& indexData);
    static WGPUShaderModule loadShaderModule(const std::filesystem::path& path, WGPUDevice device);
};