#pragma once

#include "layer.h"

#include <glm/glm.hpp>
#include <glm/ext.hpp>

class SceneLayer : public Layer
{

public:
    SceneLayer();

    ~SceneLayer();

    void onAttach();

    void onDettach();

    void onUpdate(WGPURenderPassEncoder renderPass);

    glm::mat4 rotateToTarget(glm::mat4 modelMatrix, glm::vec3 currentPosition, glm::vec3 targetPosition);

};