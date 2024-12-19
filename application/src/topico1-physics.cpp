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
    auto fm_transform = mScene.forcaMagnetica->getComponent<Transform>();
    auto carga_transform = mScene.cargaEletrica->getComponent<Transform>();
    auto fm_mesh = mScene.forcaMagnetica->getComponent<Mesh>();

    fm_mesh->initialColor = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
    if(mScene.intensidadeCarga < 0.0f)
    {
        fm_transform->initialRotation.z = 90.0f;
    }
    else
    {
        fm_transform->initialRotation.z = -90.0f;
    }

    if (mScene.intensidadeCarga == 0.0f || (int)carga_transform->initialRotation.y % 180 == 0)
    {
        fm_mesh->initialColor = glm::vec4(1.0f, 1.0f ,1.0f, 1.0f);
    }

}

