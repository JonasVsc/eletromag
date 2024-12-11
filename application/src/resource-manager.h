#pragma once
#include <vector>
#include <filesystem>
#include <webgpu/webgpu.h>
#include <glm/glm.hpp>

class ResourceManager
{
public:
    static bool loadGeometry(const std::filesystem::path& path, std::vector<float>& pointData, std::vector<uint16_t>& indexData, int dimensions);
    static WGPUShaderModule loadShaderModule(const std::filesystem::path& path, WGPUDevice device);


    struct VertexAttributes {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec3 color;
        glm::vec2 uv;
    };
    
};