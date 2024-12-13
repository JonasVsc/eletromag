#pragma once
#include <vector>
#include <filesystem>
#include <webgpu/webgpu.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

class ResourceManager
{
public:

    struct VertexAttributes {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec3 color;
    };
    
    static bool loadGeometry(const std::filesystem::path& path, std::vector<float>& pointData, std::vector<uint16_t>& indexData, int dimensions);

    static bool loadGeometryFromObj(const std::filesystem::path& path, std::vector<VertexAttributes>& vertexData);

    static WGPUShaderModule loadShaderModule(const std::filesystem::path& path, WGPUDevice device);

    static WGPUTexture loadTexture(const std::filesystem::path& path, WGPUDevice device, WGPUTextureView* pTextureView = nullptr);
};