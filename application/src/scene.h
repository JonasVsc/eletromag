#pragma once

#include <iostream>
#include <vector>

#include "object.h"
#include "layer.h"

class Scene
{

public:

    Scene(std::string debugName) : mDebugName(debugName)
    {
    }

    virtual ~Scene() = default;

    void addObject(Object* obj);

    Object* getObjectByDebugName(const std::string& debugName);

    void render(WGPURenderPassEncoder renderPass);

    inline std::string getDebugName() { return mDebugName; }

    std::vector<Object*> mObjects;

    std::string mDebugName;

    Layer* physicsLayer;
    Layer* guiLayer;

};