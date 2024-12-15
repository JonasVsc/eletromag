#include "scene-layer.h"

#include "object.h"
#include "application.h"

SceneLayer::SceneLayer()
    : Layer("SceneLayer")
{
}

SceneLayer::~SceneLayer()
{

}

void SceneLayer::onAttach()
{
    Renderer2& renderer = Application::get().getRenderer();

    float electronDirection[3] = {0.0f, 0.0f, 1.0f};
    float fieldDirection[3] = {1.0f, 0.0f, 0.0f};
    float fmDirection[3] = {0.0f, 1.0f, 0.0f};

    // create objs
    // -----------
    Object electron;
    glm::mat4 model(1.0f);
    model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    model = rotateToTarget(model, glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    electron.mUniform.modelMatrix = model;
    electron.configure("C:/Dev/eletromag/application/resources/sphere_with_vector.obj");

    Object magnetic_field;
    model = glm::mat4(1.0f);
    model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    model = rotateToTarget(model, glm::vec3(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    magnetic_field.mUniform.color = { 1.0f, 0.0f, 0.0f, 1.0f };
    magnetic_field.configure("C:/Dev/eletromag/application/resources/field.obj");

    Object fmVec;
    model = glm::mat4(1.0f);
    model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    model = rotateToTarget(model, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    fmVec.mUniform.color = { 0.0f, 1.0f, 0.0f, 1.0f };
    fmVec.configure("C:/Dev/eletromag/application/resources/arrow.obj");

    // insert objs
    // -----------
    Application::sSceneObjects.push_back(electron);
    Application::sSceneObjects.push_back(magnetic_field);
    Application::sSceneObjects.push_back(fmVec);
}

void SceneLayer::onDettach()
{

}

void SceneLayer::onUpdate(WGPURenderPassEncoder renderPass)
{

}


glm::mat4 SceneLayer::rotateToTarget(glm::mat4 modelMatrix, glm::vec3 currentPosition, glm::vec3 targetPosition)
{
    // Calcular a direção para o alvo (vetor direção em relação à posição atual)
    glm::vec3 direction = glm::normalize(targetPosition - currentPosition);

    // Criar a matriz de rotação para alinhar o objeto com a direção do alvo
    glm::mat4 rotationMatrix = glm::mat4(1.0f); // Matriz identidade

    // Definir o vetor "up" (eixo Y positivo)
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);  // "Cima" é o eixo Y positivo

    // Caso o vetor direção seja quase paralelo ao vetor 'up', usamos um vetor alternativo para evitar problemas de singularidade
    if (glm::length(glm::cross(up, direction)) < 0.001f) {
        up = glm::vec3(1.0f, 0.0f, 0.0f);  // Usar o eixo X como up se a direção for quase paralela ao Y
    }

    // Calcular o eixo "right" (eixo X positivo)
    glm::vec3 right = glm::normalize(glm::cross(up, direction));  // Produto vetorial para o eixo X
    up = glm::normalize(glm::cross(direction, right));             // Eixo Y ajustado

    // Agora criamos a matriz de rotação com base nos eixos calculados
    rotationMatrix[0] = glm::vec4(right, 0.0f);  // Eixo X
    rotationMatrix[1] = glm::vec4(up, 0.0f);     // Eixo Y
    rotationMatrix[2] = glm::vec4(-direction, 0.0f); // Eixo Z (invertido, pois Z é para fora da tela)

    // Manter a parte de translação intacta, preservando a posição
    modelMatrix[3] = glm::vec4(currentPosition, 1.0f);

    // Multiplicar a rotação com a parte de rotação da modelMatrix
    modelMatrix = rotationMatrix * modelMatrix;

    return modelMatrix;
}