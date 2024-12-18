#pragma once

#include "component.h"


class Mesh : public Component 
{
public:
    glm::vec4 color = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
    glm::vec4 initialColor = color;

    void update(float deltaTime)
    {
        Renderer2& renderer = Application::get().getRenderer();

        wgpuQueueWriteBuffer(renderer.getQueue(), parent->getUniformBuffer(), offsetof(MyUniforms, color), &color, sizeof(glm::vec4));

        if(!Application::sRunningSimulation)
            color = initialColor;
    }
};