#pragma once

#include "object.h"


class Charge : public Object
{

public:

    Charge(std::string debugName, const std::filesystem::path& path)
        : Object(debugName, path)
    {
    }

    ~Charge() {}

    void physicsUpdate() override
    {
        if(!Application::get().mRunningSimulation) // idle
        {
            reset();
            return;
        }

        Physics::applyVelocity(this, mVelocityDirection, mVelocity);

        // glm::vec3 velocityDirection = glm::normalize(glm::vec3(mVelocityDirection[0], mVelocityDirection[1], mVelocityDirection[2]));
        // mPosition[0] += (float)velocityDirection.x * mVelocity * Application::get().deltaTime;
        // mPosition[1] += (float)velocityDirection.y * mVelocity * Application::get().deltaTime;
        // mPosition[2] += (float)velocityDirection.z * mVelocity * Application::get().deltaTime;
        

    }

};
