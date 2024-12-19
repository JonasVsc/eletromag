#include "topico1-physics.h"

#include "topico1-scene.h"



Topico1PhysicsLayer::Topico1PhysicsLayer(SceneTopico1& scene)
    : mScene(scene), Layer("Topico1PhysicsLayer")
{
}

Topico1PhysicsLayer::~Topico1PhysicsLayer()
{

}

void Topico1PhysicsLayer::onAttach()
{
    
}


void Topico1PhysicsLayer::onUpdate(WGPURenderPassEncoder renderPass)
{
    Renderer2 renderer = Application::get().getRenderer();
    auto tranform = mScene.forcaMagnetica->getComponent<Transform>();

}

