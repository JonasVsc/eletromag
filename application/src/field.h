#pragma once

#include "object.h"


class Field : public Object
{

public:

    float mIntensity;

    Type getType() const override { return Type::Field; }

    Field(std::string debugName, const std::filesystem::path& path)
        : Object(debugName, path)
    {
        setColor(1.0f, 0.0f, 0.0f, 1.0f);
    }

    ~Field() {}

    void physicsUpdate() override
    {
        if(!Application::get().mRunningSimulation) // idle
        {
            reset();
            return;
        }

    }

};
