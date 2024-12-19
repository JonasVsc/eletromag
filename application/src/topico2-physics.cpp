#include "topico2-physics.h"

#include "topico2-scene.h"

Topico2PhysicsLayer::Topico2PhysicsLayer(SceneTopico2& scene)
    : mScene(scene), Layer("Topico2PhysicsLayer")
{
}

Topico2PhysicsLayer::~Topico2PhysicsLayer()
{

}

void Topico2PhysicsLayer::onAttach()
{
    std::cout << "Objects loaded: ";
    for(auto& obj : mScene.mObjects)
    {
        std::cout << obj->getDebugName() << '\n';
    }
    std::cout << '\n';
}

void Topico2PhysicsLayer::onDettach()
{

}

void Topico2PhysicsLayer::onUpdate(WGPURenderPassEncoder renderPass)
{
}