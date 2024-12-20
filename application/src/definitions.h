#pragma once

#include <glm/glm.hpp>
#include <glm/ext.hpp>

struct MyUniforms {
    glm::mat4x4 projectionMatrix;
    glm::mat4x4 viewMatrix;
    glm::mat4x4 modelMatrix;
    glm::vec4 color;
};

static_assert(sizeof(MyUniforms) % 16 == 0);

constexpr float PI = 3.14159265358979323846f;
