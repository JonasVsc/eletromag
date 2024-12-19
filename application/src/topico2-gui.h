#pragma once

#include "layer.h"

#include <imgui.h>
#include <backends/imgui_impl_wgpu.h>
#include <backends/imgui_impl_glfw.h>

class SceneTopico2;

class Topico2GUI : public Layer
{

public:
    SceneTopico2& mScene;

public:

    Topico2GUI(SceneTopico2& scene) 
        : mScene(scene), Layer("Topico2GUILayer")
    {

    };

    ~Topico2GUI() {};

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

        ImGui::SetNextWindowSize(ImVec2(450, 250), ImGuiCond_Once); 
        ImGui::Begin("Calculadora de Alguma coisa");

        ImGui::Text("Força Alguma coisa:");

        ImGui::End();
    }

};
