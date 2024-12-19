#include "topico1-gui.h"

#include "layer.h"
#include "physics.h"
#include "transform.h"
#include "topico1-scene.h"

#include <imgui.h>
#include <backends/imgui_impl_wgpu.h>
#include <backends/imgui_impl_glfw.h>


Topico1GUI::Topico1GUI(SceneTopico1& scene) 
    : mScene(scene), Layer("Topico1GUILayer")
{

};

Topico1GUI::~Topico1GUI() {};

void Topico1GUI::onAttach()
{

}

void Topico1GUI::onUpdate(WGPURenderPassEncoder renderPass)
{
    auto cargaEletrica_transform = mScene.cargaEletrica->getComponent<Transform>();
    
    static float& carga = mScene.intensidadeCarga;
    static float velocidade = 1.0f;
    static float campoMagnetico = 1.0f;
    static float forca = 0.0f;
    static float& anguloGraus = cargaEletrica_transform->initialRotation.y;

    ImGui::SetNextWindowSize(ImVec2(450, 250), ImGuiCond_Once); 
    ImGui::Begin("Calculadora de Força Magnética");

    ImGui::InputFloat("Carga (C)", &carga, 10.0f, 10.0f, "%.12f");
    ImGui::InputFloat("Velocidade (m/s)", &velocidade, 10.0f, 10.0f, "%.12f");
    ImGui::InputFloat("Campo Magnético (T)", &campoMagnetico, 10.0f, 10.0f, "%.12f");
    ImGui::InputFloat("Ângulo (graus)", &anguloGraus, 10.0f, 10.0f);

    if (ImGui::Button("Calcular Força")) {
        forca = Physics::calcularForcaMagnetica(carga, velocidade, campoMagnetico, anguloGraus);
    }

    ImGui::Text("Força Magnética: %.12f N", forca);

    ImGui::End();
}

