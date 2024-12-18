#pragma once

#include "application.h"
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
       
        model = glm::translate(model, position);
        model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, scale);
      
        wgpuQueueWriteBuffer(renderer.getQueue(), parent->getUniformBuffer(), offsetof(MyUniforms, modelMatrix), &model, sizeof(glm::mat4));

        if(!Application::sRunningSimulation)
        {
            position = initialPosition;
            position = initialPosition;
            position = initialPosition;
            rotation = initialRotation;
            rotation = initialRotation;
            rotation = initialRotation;
            scale = initialScale;
            scale = initialScale;
            scale = initialScale;
        }
    }
};