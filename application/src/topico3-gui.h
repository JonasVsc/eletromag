#pragma once

#include "layer.h"

#include <imgui.h>
#include <backends/imgui_impl_wgpu.h>
#include <backends/imgui_impl_glfw.h>

class SceneTopico3;

class Topico3GUI : public Layer
{

public:
    SceneTopico3& mScene;

public:

    Topico3GUI(SceneTopico3& scene) 
        : mScene(scene), Layer("Topico3GUILayer")
    {

    };

    ~Topico3GUI() {};

    void onAttach()
    {

    }

    void onUpdate(WGPURenderPassEncoder renderPass)
    {

        ImGui::SetNextWindowSize(ImVec2(450, 250), ImGuiCond_Once); 
        ImGui::Begin("Calculadora Efeito Hall");


        ImGui::End();
    }

};
