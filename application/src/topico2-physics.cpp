#include "topico2-physics.h"

#include "topico2-scene.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

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
    Renderer2 renderer = Application::get().getRenderer();

    auto apontador_transform = mScene.apontador->getComponent<Transform>();
    auto qualquer_transform = mScene.qualquer->getComponent<Transform>();

    glm::vec3 direction = glm::normalize(qualquer_transform->initialPosition - apontador_transform->initialPosition);

    glm::vec3 defaultForward(-1.0f ,0.0f, 0.0f);
    glm::quat rotation = glm::rotation(defaultForward, direction);

    mScene.apontador->getUniform().modelMatrix = glm::mat4_cast(rotation);
    mScene.apontador->getUniform().modelMatrix = glm::translate(mScene.apontador->getUniform().modelMatrix, apontador_transform->initialPosition);

    wgpuQueueWriteBuffer(renderer.getQueue(), mScene.apontador->getUniformBuffer(), offsetof(MyUniforms, modelMatrix), glm::value_ptr(mScene.apontador->getUniform().modelMatrix), sizeof(glm::mat4));
}