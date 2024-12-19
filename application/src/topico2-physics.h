#pragma once

#include "layer.h"

class SceneTopico2;

class Topico2PhysicsLayer : public Layer
{

public:
    SceneTopico2& mScene;

public:

    Topico2PhysicsLayer(SceneTopico2& scene);

    ~Topico2PhysicsLayer();

    void onAttach();

    void onDettach();

    void onUpdate(WGPURenderPassEncoder renderPass);

};
