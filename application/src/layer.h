#pragma once

#include <iostream>

class Layer
{

public:
    Layer(const std::string& debugName);

    virtual ~Layer();

    virtual void onAttach() {}

    virtual void onDettach() {}

    virtual void onUpdate() {}

    inline std::string getDebugName() { return mDebugName; }

protected:

    std::string mDebugName;

};