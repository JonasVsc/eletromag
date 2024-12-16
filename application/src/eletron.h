#pragma once

#include "object.h"
#include "field.h"


class Charge : public Object
{

public:

    Charge(std::string debugName, const std::filesystem::path& path)
        : Object(debugName, path)
    {
        setColor(0.0f, 0.0f, 1.0f, 1.0f);
        setPosition(2.5f, 2.0f, -3.0f);
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

    }

};
