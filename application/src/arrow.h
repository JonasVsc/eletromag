#pragma once

#include "object.h"
#include "application.h"

class Arrow : public Object
{

public:

    float mIntensity = 0.0f;
    float mDirection[3] {0.0f, 0.0f, 0.0f};

    Type getType() const override { return Type::Arrow; }

    Arrow(std::string debugName, const std::filesystem::path& path)
        : Object(debugName, path)
    {
        setColor(0.0f, 1.0f, 0.0f, 0.0f);
    }

    ~Arrow() {}

    void physicsUpdate() override
    {
        isVisible(false);
        followObject();



        if(!Application::get().mRunningSimulation) // idle
        {
            reset();
            return;
        }
        isVisible(true);


    }

    void isVisible(bool value)
    {
        if(value)
            setScale(1.0f, 1.0f, 1.0f);
        else
            setScale(0.0f, 0.0f, 0.0f);
    }

    void followObject()
    {
        Object* obj = Application::get().mCurrentScene->getObjectByDebugName("Charge");

        if(!obj)
        {
            std::cout << "NULLPTR ERROR" << '\n';
            return;
        }
        setPosition(obj->mPosition[0], obj->mPosition[1], obj->mPosition[2]);
        setRotation(obj->mRotation[0], obj->mRotation[1], obj->mRotation[2]);
    }
  

};
