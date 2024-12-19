#pragma once

#include "layer.h"

class SceneTopico3;

class Topico3PhysicsLayer : public Layer
{

public:
    SceneTopico3& mScene;

public:

    Topico3PhysicsLayer(SceneTopico3& scene);

    ~Topico3PhysicsLayer();

    void onAttach();

    void onDettach();

    void onUpdate(WGPURenderPassEncoder renderPass);

};
