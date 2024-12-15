#pragma once

#include <iostream>
#include <vector>

#include "object.h"

class Scene
{

public:

    Scene(std::string debugName) : mDebugName(debugName)
    {
    }

    virtual ~Scene() = default;

    void addObject(Object& obj);

    virtual void load() {};

    inline std::string getDebugName() { return mDebugName; }

    std::vector<Object> mObjects;

    std::string mDebugName;

};