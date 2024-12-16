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

        std::cout << "testando charger!" << '\n';
    }

};
