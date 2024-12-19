#pragma once

#include "layer.h"

class SceneTopico1;

class Topico1PhysicsLayer : public Layer
{

public:
    SceneTopico1& mScene;

public:

    Topico1PhysicsLayer(SceneTopico1& scene);

    ~Topico1PhysicsLayer();

    void onAttach();

    void onUpdate(WGPURenderPassEncoder renderPass);

};
