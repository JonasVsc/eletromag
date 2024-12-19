#pragma once

#include "layer.h"

#include <imgui.h>
#include <backends/imgui_impl_wgpu.h>
#include <backends/imgui_impl_glfw.h>

class SceneTopico1;

class Topico1GUI : public Layer
{

public:
    SceneTopico1& mScene;

public:

    Topico1GUI(SceneTopico1& scene) 
        : mScene(scene), Layer("Topico1GUILayer")
    {

    };

    ~Topico1GUI() {};

    void onAttach()
    {

    }

    float calcularForcaMagnetica(float carga, float velocidade, float campoMagnetico, float anguloGraus) 
    {
        float anguloRadianos = anguloGraus * (M_PI / 180.0f);
        return carga * velocidade * campoMagnetico * sin(anguloRadianos);
    }

    void onUpdate(WGPURenderPassEncoder renderPass)
    {
        static float carga = 1.0f;
        static float velocidade = 1.0f;
        static float campoMagnetico = 1.0f;
        static float anguloGraus = 0.0f;
        static float forca = 0.0f;

        ImGui::SetNextWindowSize(ImVec2(450, 250), ImGuiCond_Once); 
        ImGui::Begin("Calculadora de Força Magnética");

        ImGui::InputFloat("Carga (C)", &carga, 0.1f, 10.0f);
        ImGui::InputFloat("Velocidade (m/s)", &velocidade, 0.1f, 100.0f);
        ImGui::InputFloat("Campo Magnético (T)", &campoMagnetico, 0.01f, 10.0f);
        ImGui::InputFloat("Ângulo (graus)", &anguloGraus, 0.01f, 3.14f);

        if (ImGui::Button("Calcular Força")) {
            forca = calcularForcaMagnetica(carga, velocidade, campoMagnetico, anguloGraus);
        }

        ImGui::Text("Força Magnética: %f N", forca);

        ImGui::End();
    }

};
