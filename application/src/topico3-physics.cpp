#include "topico3-physics.h"

#include "topico3-scene.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

Topico3PhysicsLayer::Topico3PhysicsLayer(SceneTopico3& scene)
    : mScene(scene), Layer("Topico3PhysicsLayer")
{
}

Topico3PhysicsLayer::~Topico3PhysicsLayer()
{

}

void Topico3PhysicsLayer::onAttach()
{
    std::cout << "Objects loaded: ";
    for(auto& obj : mScene.mObjects)
    {
        std::cout << obj->getDebugName() << '\n';
    }
    std::cout << '\n';
}

void Topico3PhysicsLayer::onDettach()
{

}

void Topico3PhysicsLayer::onUpdate(WGPURenderPassEncoder renderPass)
{
    Renderer2 renderer = Application::get().getRenderer();
    auto carga_transform = mScene.carga->getComponent<Transform>();
    glm::mat4& carga_modelMatrix = mScene.carga->getUniform().modelMatrix;
    float time = glfwGetTime();




}