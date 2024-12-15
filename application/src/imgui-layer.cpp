#include "imgui-layer.h"

#include "application.h"

#include <imgui.h>
#include <backends/imgui_impl_wgpu.h>
#include <backends/imgui_impl_glfw.h>

#include <optional>


ImGuiLayer::ImGuiLayer()
    : Layer("ImGuiLayer")
{
}

ImGuiLayer::~ImGuiLayer()
{

}

void ImGuiLayer::onAttach()
{
    std::cout << "ImGuiLayer Attached!" << '\n';

    Application& app = Application::get();
    Renderer2& renderer = app.getRenderer();
    GLFWwindow* window = static_cast<GLFWwindow*>(app.getWindow().getNativeWindow());

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::GetIO();

    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4 customColor = ImVec4(0.1f, 0.5f, 0.8f, 1.0f);
    style.Colors[ImGuiCol_Button] = customColor;
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.2f, 0.6f, 0.9f, 1.0f);
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.0f, 0.4f, 0.7f, 1.0f);
    style.WindowPadding = ImVec2(10.0f, 10.0f);  
    style.FramePadding = ImVec2(5.0f, 5.0f);     
    style.ItemSpacing  = ImVec2(8.0f, 4.0f);     
    style.ScrollbarSize = 15.0f;                 
    style.WindowRounding = 5.0f;    
    style.FrameRounding = 4.0f;     
    style.PopupRounding = 3.0f;     
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10, 10));
    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->AddFontFromFileTTF("C:/Dev/eletromag/application/resources/RobotoMono-VariableFont_wght.ttf", 16.0f);

    ImGui_ImplGlfw_InitForOther(window, true);
    ImGui_ImplWGPU_InitInfo init_info;
    init_info.Device = renderer.getDevice();
    init_info.NumFramesInFlight = 3;
    init_info.RenderTargetFormat = WGPUTextureFormat_BGRA8Unorm;
    init_info.DepthStencilFormat = WGPUTextureFormat_Depth24Plus;
    ImGui_ImplWGPU_Init(&init_info);
}

void ImGuiLayer::onDetach()
{
    ImGui_ImplGlfw_Shutdown();
    ImGui_ImplWGPU_Shutdown();
}

void ImGuiLayer::onUpdate(WGPURenderPassEncoder renderPass)
{
    ImGui_ImplWGPU_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    objectTreeGUI();
    sceneGUI();
    fpsGUI();

    ImGui::EndFrame();
    ImGui::Render();
    ImGui_ImplWGPU_RenderDrawData(ImGui::GetDrawData(), renderPass);
}

void ImGuiLayer::sceneGUI()
{
    ImGui::SetNextWindowSize(ImVec2(300, 720), ImGuiCond_Once); 
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSizeConstraints(ImVec2(300, 200), ImVec2(300, 200));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0)); 
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f); 

    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0)); 

    ImGui::Begin("Cena", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);

    auto& scenes = Application::get().mScenes;
    ImGui::PopStyleVar(3); 
    ImGui::PopStyleColor(1);  
    

    if (ImGui::BeginCombo("##Scene", Application::get().mCurrentScene->getDebugName().c_str())) {
        for(auto it = scenes.begin(); it != scenes.end(); it++)
        {
            if (ImGui::Selectable(it->first.c_str())) {
                Application::get().setCurrentScene(it->second);
            }
        }
        ImGui::EndCombo();
    }
    ImGui::End();
    
}

void ImGuiLayer::objectTreeGUI()
{
    static Scene* previousScene = nullptr;
    Scene* scene = Application::get().mCurrentScene;

    ImGui::SetNextWindowSize(ImVec2(300, 720), ImGuiCond_Once); 
    ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x - 300, 0));
    ImGui::SetNextWindowSizeConstraints(ImVec2(300, 720), ImVec2(300, 720));
    ImGui::Begin("Objetos", nullptr, ImGuiWindowFlags_NoTitleBar);

    float listBoxHeight = ImGui::GetTextLineHeightWithSpacing() * 5; 
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4.0f, 2.0f));

    static int selected = -1;
    if(scene != previousScene)
    {
        selected = -1;
        previousScene = scene;
    }
    ImGui::Text("Objetos");
    if(ImGui::BeginListBox(("##" + scene->getDebugName()).c_str(), ImVec2(-1, listBoxHeight))) 
    {
        for(size_t i = 0; i < scene->mObjects.size(); i++)
        {
            const bool isSelected = (selected == i);
            if(ImGui::Selectable(scene->mObjects.at(i).getDebugName().c_str(), isSelected))
            {
                selected = i;
                std::cout << "Selecionado: " << scene->mObjects.at(selected).getDebugName() << std::endl;
            }
        }

        ImGui::EndListBox();
    }
    ImGui::PopStyleVar(1);


    if(selected != -1)
    {
        auto& obj = scene->mObjects.at(selected);
        auto objLabel = obj.getDebugName();
        ImGui::Text("Propriedades");
        ImGui::SetNextItemOpen(true, ImGuiCond_Once);

        if(ImGui::TreeNode("Transform"))
        {
            auto Id = "##" + objLabel;
            ImGui::Text("Position:");
            ImGui::DragFloat3((Id + "Position").c_str(), obj.mPosition, 0.1);
            ImGui::Text("Rotation:");
            ImGui::DragFloat3((Id + "Rotation").c_str(), obj.mRotation, 0.1);
            ImGui::Text("Scale:");
            ImGui::DragFloat3((Id + "Scale").c_str(), obj.mScale, 0.1);
            ImGui::TreePop();
        }
        if(ImGui::TreeNode("Material"))
        {
            auto Id = "##" + objLabel;
            ImGui::Text("Color:");
            ImGui::ColorEdit4((Id + "Color").c_str(), (float*)&obj.mColor);
            ImGui::TreePop();
        }
    }

    ImGui::End();
}


void ImGuiLayer::fpsGUI()
{
    ImVec2 windowPos = ImVec2(0, ImGui::GetIO().DisplaySize.y - 30);
    ImVec2 windowSize = ImVec2(100, 30); 

    ImGui::SetNextWindowPos(windowPos);
    ImGui::SetNextWindowSize(windowSize);
    ImGui::SetNextWindowBgAlpha(0.0f);  
    ImGui::Begin("FPS Window", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoInputs);
    float fps = ImGui::GetIO().Framerate;
    ImGui::Text("FPS: %.f", fps);
    ImGui::End();
}

