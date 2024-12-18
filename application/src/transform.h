#pragma once

#include "component.h"


class Transform : public Component 
{
public:
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 rotation = glm::vec3(0.0f);
    glm::vec3 scale = glm::vec3(1.0f);

    glm::vec3 initialPosition = position;
    glm::vec3 initialRotation = rotation;
    glm::vec3 initialScale = scale;

    void update(float deltaTime)
    {
        Renderer2& renderer = Application::get().getRenderer();
        glm::mat4 model(1.0f);

        if(Application::sRunningSimulation)
        {
            model = glm::translate(model, position);
            model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
            model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
            model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
            model = glm::scale(model, scale);
        } 
        else
        {
            model = glm::translate(model, initialPosition);
            model = glm::rotate(model, glm::radians(initialRotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
            model = glm::rotate(model, glm::radians(initialRotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
            model = glm::rotate(model, glm::radians(initialRotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
            model = glm::scale(model, initialScale);
        }


        wgpuQueueWriteBuffer(renderer.getQueue(), parent->getUniformBuffer(), offsetof(MyUniforms, modelMatrix), &model, sizeof(glm::mat4));
    }
};