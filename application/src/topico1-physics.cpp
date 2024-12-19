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

void printMatrix(const glm::mat4& matrix) {
    // Imprime os valores da matriz
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            std::cout << matrix[i][j] << " ";
        }
        std::cout << std::endl;
    }
}

void Topico1PhysicsLayer::onUpdate(WGPURenderPassEncoder renderPass)
{
    Renderer2 renderer = Application::get().getRenderer();
    auto tranform = mScene.forcaMagnetica->getComponent<Transform>();

    // Posição do objeto e direção de destino
    glm::vec3 position = tranform->initialPosition;  // Posição do objeto
    glm::vec3 target(0.0f, 0.0f, 1.0f);         // Direção para onde o objeto deve olhar
    glm::vec3 up(0.0f, 1.0f, 0.0f);                // Vetor "up" (normalmente o eixo Y)

    // Calcular a direção do objeto (vetor que vai do objeto até o alvo)
    glm::vec3 direction = glm::normalize(target - position);

    // Calcular a matriz de rotação usando a direção
    glm::mat4 rotation = glm::mat4(1.0f);  // Começa com a matriz identidade
    rotation = glm::lookAt(glm::vec3(0.0f), direction, up);  // Gera a rotação que alinha o objeto com a direção

    // Atualizar a matriz do modelo com a rotação calculada
    mScene.forcaMagnetica->getUniform().modelMatrix = glm::translate(glm::mat4(1.0f), position) * rotation;

    // Enviar a matriz do modelo para o shader
    wgpuQueueWriteBuffer(renderer.getQueue(), mScene.forcaMagnetica->getUniformBuffer(),
                         offsetof(MyUniforms, modelMatrix), glm::value_ptr(mScene.forcaMagnetica->getUniform().modelMatrix), sizeof(glm::mat4));

    // Imprimir a matriz do modelo para depuração
    printMatrix(mScene.forcaMagnetica->getUniform().modelMatrix);
}

