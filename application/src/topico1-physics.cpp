#include "topico1-physics.h"

#include "topico1-scene.h"

Topico1PhysicsLayer::Topico1PhysicsLayer(SceneTopico1* scene)
    : mScene(scene), Layer("Topico1PhysicsLayer")
{
}

Topico1PhysicsLayer::~Topico1PhysicsLayer()
{

}

void Topico1PhysicsLayer::onAttach()
{
    std::cout << "Objects loaded: ";
    for(auto& obj : mScene->mObjects)
    {
        std::cout << obj->getDebugName() << '\n';
    }
    std::cout << '\n';
}

void Topico1PhysicsLayer::onDettach()
{

}

void Topico1PhysicsLayer::onUpdate(WGPURenderPassEncoder renderPass)
{
    std::cout << mScene->teste->getDebugName() << '\n';
}